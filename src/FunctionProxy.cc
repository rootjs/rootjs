#include "FunctionProxy.h"

#include "FunctionProxyFactory.h"
#include "ObjectProxyFactory.h"
#include "ObjectProxy.h"
#include "BooleanProxy.h"
#include "NumberProxy.h"
#include "StringProxy.h"
#include "FunctionInfo.h"
#include "PointerInfo.h"
#include "Toolbox.h"

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <v8.h>

#include <TROOT.h>
#include <TClassTable.h>
#include <TInterpreter.h>
#include <TCollection.h>
#include <TIterator.h>
#include <TList.h>
#include <TClassRef.h>
#include <TFunction.h>
#include <TMethodArg.h>

namespace rootJS
{
	std::map<TFunction*, CallFunc_t*> FunctionProxy::functions;
	std::map<std::string, mappedTypes> FunctionProxy::typeMap = {
		{"char", mappedTypes::CHAR},
		{"TString", mappedTypes::TSTRING},
		{"Int_t", mappedTypes::INT},
		{"int", mappedTypes::INT},
		{"Double_t", mappedTypes::DOUBLE},
		{"Bool_t", mappedTypes::BOOL},
		//{"_t", mappedTypes::CHAR}
	};

	std::vector<TFunction*> FunctionProxy::getMethodsFromName(TClassRef scope, std::string name)
	{
		std::vector<TFunction*> methods;

		const TList *funcList = scope.GetClass()->GetListOfAllPublicMethods();
		TIter funcIter(funcList);
		TFunction *func;
		while((func = (TFunction*)funcIter()))
		{
			if(name.compare(func->GetName()) == 0)
			{
				methods.push_back(func);
			}
		}

		return methods;
	}

	FunctionProxy::FunctionProxy(FunctionInfo &info, TFunction *function, TClass *scope) : Proxy(info, scope)
	{
		this->function = function;
	}

	FunctionProxy* FunctionProxy::clone()
	{
		FunctionProxy *p = new FunctionProxy(*(rootJS::FunctionInfo*)info, function, scope);
		p->buf = buf;
		p->facePtr = facePtr;
		return p;
	}

	CallFunc_t* FunctionProxy::getCallFunc(const TClassRef& classRef, TFunction* method)
	{
		std::map<TFunction*, CallFunc_t*>::iterator iterator = FunctionProxy::functions.find(method);
		if (iterator != FunctionProxy::functions.end())
		{
			return iterator->second;
		}
		else
		{
			CallFunc_t* callf = nullptr;
			TFunction* func = (TFunction*)method;
			std::string callString = "";

			ClassInfo_t* gcl;
			gcl = classRef.GetClass() ? classRef->GetClassInfo() : nullptr;
			if ( ! gcl )
			{
				gcl = gInterpreter->ClassInfo_Factory();
			}

			TCollection* method_args = func->GetListOfMethodArgs();
			TIter iarg( method_args );

			TMethodArg* method_arg = 0;
			while ((method_arg = (TMethodArg*)iarg.Next()))
			{
				std::string fullType = method_arg->GetTypeNormalizedName();
				if ( callString.empty() )
					callString = fullType;
				else
					callString += ", " + fullType;
			}

			Long_t offset = 0;
			callf = gInterpreter->CallFunc_Factory();

			gInterpreter->CallFunc_SetFuncProto(
			    callf,
			    gcl,
			    func ? func->GetName() : classRef->GetName(),
			    callString.c_str(),
			    func ? (func->Property() & kIsConstMethod) : kFALSE,
			    &offset,
			    ROOT::kExactMatch );

			/*
			 * delete gcl;
			 *
			 * TODO? gcl should be deleted, however ClassInfo_t is an,
			 * incomplete type. I didn't find its definition (even the ROOT
			 * docs do not link this type). :(
			 */

			/*
			 * Workaround ;)
			 * TString* string = (TString*) gROOT->ProcessLineFast("new TString(\"test\");");
			 *	Toolbox::logInfo(std::string(string->Data()));
			 */


			if ( ! gInterpreter->CallFunc_IsValid( callf ) )
			{
				gInterpreter->CallFunc_SetFuncProto(
				    callf,
				    gcl,
				    func ? func->GetName() : classRef->GetName(),
				    callString.c_str(),
				    func ? (func->Property() & kIsConstMethod) : kFALSE,
				    &offset );
				return callf;
			}
			else
			{
				return callf;
			}

			return nullptr;

		}
	}

	void FunctionProxy::prepareCall(const  v8::Local<v8::Array>& args)
	{
		CallFunc_t* callFunc = (CallFunc_t*)info->getAddress();
		if(!callFunc)
		{
			//TODO Handle this, should not segfault (maybe throw something...)
		}
		this->facePtr = gCling->CallFunc_IFacePtr( callFunc );
		gCling->CallFunc_Delete(callFunc);

		buf = std::vector<void*>( args->Length() );
		for(int i = 0; i < (int)args->Length(); i++)
		{
			void** bufEl = (void**)malloc(sizeof(void*));
			*bufEl = bufferParam((TMethodArg*)(function->GetListOfMethodArgs()->At(i)), args->Get(i));
			buf[i] = bufEl;
		}
	}

	ObjectProxy* FunctionProxy::call(void *self, bool isConstructorCall /* false */)
	{
		void *result = nullptr;
		void **resultPtr;
		resultPtr = &result;
		void *allocated = nullptr;
		if(!isConstructorCall) {
			const char *returnType = function->GetReturnTypeName();
			DictFuncPtr_t dictFunc = gClassTable->GetDict(returnType);
			if(dictFunc) {
				TClass *klass = dictFunc();
				result = malloc(klass->Size());
				allocated = result;
				resultPtr = (void**)result;
			}
		}

		switch(facePtr.fKind)
		{
		case (TInterpreter::CallFuncIFacePtr_t::kGeneric):
			facePtr.fGeneric(self, buf.size(), buf.data(), resultPtr);
			break;
		case (TInterpreter::CallFuncIFacePtr_t::kCtor):
			facePtr.fCtor(buf.data(), &result, buf.size());
			break;
		case (TInterpreter::CallFuncIFacePtr_t::kDtor):
			throw std::invalid_argument("Destructor calls are not supported, objects will automatically be destructed when they go out of scope.");
			break;
		case (TInterpreter::CallFuncIFacePtr_t::kUninitialized):
			throw std::invalid_argument("Got an uninitialized CallFuncIFacePtr_t, this should not happen and might be a bug.");
			break;
		}

		for(int i = 0; i < (int)buf.size(); i++)
		{
			free(*((void**)buf[i]));
			free((void*)buf[i]);
		}

		ObjectProxy* proxy;
		PointerInfo mode(result, function->GetReturnTypeName(), 1);
		proxy = ObjectProxyFactory::createObjectProxy(mode, TClassRef());

		if(proxy)
		{
			if(allocated) {
				proxy->registerMallocedSpace(allocated);
			}
			return proxy;
		} else if(allocated) {
			free(allocated);
		}

		return nullptr;
	}

	void* FunctionProxy::bufferParam(TMethodArg* arg, v8::Local<v8::Value> originalArg)
	{
		std::map<std::string, mappedTypes>::iterator iterator = typeMap.find(std::string(arg->GetTypeName()));
		if(iterator == typeMap.end())
		{
			Toolbox::throwException(std::string("bufferParam does not know how to handle ") + arg->GetTypeName());
			return nullptr;
		}
		switch(iterator->second)
		{
		case mappedTypes::CHAR:
			return argToChar(originalArg);
		case mappedTypes::INT:
			return argToInt(originalArg);
		case mappedTypes::DOUBLE:
			return argToDouble(originalArg);
		case mappedTypes::BOOL:
			return argToBool(originalArg);
		case mappedTypes::TSTRING:
			return argToTString(originalArg);
		}

		//TODO: This will explode - huge fireball
		return nullptr;
	}

	char* FunctionProxy::argToChar(v8::Local<v8::Value> originalArg)
	{
		v8::String::Utf8Value string(originalArg->ToString());
		char *str = (char *) malloc(string.length() + 1);
		strcpy(str, *string);
		return str;
	}

	double* FunctionProxy::argToDouble(v8::Local<v8::Value> originalArg)
	{
		double *doubleValue = (double*)malloc(sizeof(double));
		*doubleValue = getDoubleFromArg(originalArg);
		return doubleValue;
	}

	int* FunctionProxy::argToInt(v8::Local<v8::Value> originalArg)
	{
		int *intValue = (int *)malloc(sizeof(int));
		*intValue = (int)getDoubleFromArg(originalArg);
		return intValue;
	}

	bool* FunctionProxy::argToBool(v8::Local<v8::Value> originalArg)
	{
		bool* boolValue = (bool*)malloc(sizeof(bool));
		if(originalArg->IsBoolean())
		{
			*boolValue = v8::Local<v8::Boolean>::Cast(originalArg)->Value();
		}
		else
		{
			*boolValue = v8::Local<v8::BooleanObject>::Cast(originalArg)->ValueOf();
		}
		return boolValue;
	}

	TString* FunctionProxy::argToTString(v8::Local<v8::Value> originalArg)
	{
		v8::String::Utf8Value string(originalArg->ToString());
		return new TString(*string);
	}

	double FunctionProxy::getDoubleFromArg(v8::Local<v8::Value> originalArg)
	{
		if(originalArg->IsNumber())
		{
			return v8::Local<v8::Number>::Cast(originalArg)->Value();
		}
		else if(originalArg->IsNumberObject())
		{
			return v8::Local<v8::NumberObject>::Cast(originalArg)->ValueOf();
		}
		else
		{
			return -1;
		}
	}

}
