#include "FunctionProxy.h"
#include "BooleanProxy.h"
#include "NumberProxy.h"
#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "StringProxy.h"
#include "Toolbox.h"
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <v8.h>

#include <TClassRef.h>
#include <TInterpreter.h>

#include <TROOT.h>
#include <TCollection.h>
#include <TFunction.h>
#include <TIterator.h>
#include <TList.h>
#include <TMethodArg.h>
#include "FunctionInfo.h"
#include "FunctionProxyFactory.h"
#include "PointerInfo.h"

namespace rootJS {
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
			if ( ! gcl ) {
				gcl = gInterpreter->ClassInfo_Factory();
			}

			TCollection* method_args = func->GetListOfMethodArgs();
			TIter iarg( method_args );

			TMethodArg* method_arg = 0;
			while ((method_arg = (TMethodArg*)iarg.Next())) {
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


			if ( ! gInterpreter->CallFunc_IsValid( callf ) ) {
				gInterpreter->CallFunc_SetFuncProto(
				    callf,
				    gcl,
				    func ? func->GetName() : classRef->GetName(),
				    callString.c_str(),
				    func ? (func->Property() & kIsConstMethod) : kFALSE,
				    &offset );
				return callf;
			} else {
				return callf;
			}

			return nullptr;

		}
	}

	std::vector<TFunction*> FunctionProxy::getMethodsFromName(TClassRef scope, std::string name)
	{
		std::vector<TFunction*> methods;

		const TList *funcList = scope.GetClass()->GetListOfAllPublicMethods();
		TIter funcIter(funcList);
		TFunction *func;
		while((func = (TFunction*)funcIter())) {
			if(name.compare(func->GetName()) == 0) {
				methods.push_back(func);
			}
		}

		return methods;
	}

	FunctionProxy::FunctionProxy(void* address, FunctionInfo& mode, TFunction* function, TClassRef scope)
		: Proxy(mode, scope)
	{
		this->address = address;
		this->function = function;
	}

	std::vector<ObjectProxy*> FunctionProxy::validateArgs(v8::FunctionCallbackInfo<v8::Value> args)
	{
		std::vector<ObjectProxy*> validatedArgs;

		TFunction method = *function;
		if (method.GetNargs() <= args.Length() && args.Length() <= method.GetNargsOpt())
		{
			TList *expectedArgs = method.GetListOfMethodArgs();
			for (int i = 0; i < args.Length(); i++)
			{
				TMethodArg *expectedArg = static_cast<TMethodArg*>(expectedArgs->At(i));

				// Check if the argument is a JavaScript object
				if (args[i]->IsObject())
				{
					v8::Object *objectArg = static_cast<v8::Object*>(*args[i]);
					if (objectArg->InternalFieldCount() > 0)
					{
						// TODO validate arg
						void *arg = objectArg->GetAlignedPointerFromInternalField(Toolbox::v8ObjectInternalField::ObjectProxyPtr);
						validatedArgs.push_back(static_cast<ObjectProxy*>(arg));
					}
					else
					{
						std::ostringstream msgStream;
						msgStream << "Error while validating arg " << i << ": v8::Object at " << &(*(*args[i])) << " has no internal fields";
						Toolbox::throwException(msgStream.str());
					}
				}
				else
				{
					// Else, it must be a JavaScript primitive
					if (args[i]->IsBoolean())
					{
						// TODO proper type validation
						if (BooleanProxy::isBoolean(expectedArg->GetTypeName()))
						{
							v8::Boolean *booleanArg = static_cast<v8::Boolean*>(*args[i]);
							bool value = booleanArg->Value();
							// TODO push_back ObjectProxy*
						}
						else
						{
							std::ostringstream msgStream;
							msgStream << "Error while validating arg " << i << ": Expected " << expectedArg->GetTypeNormalizedName() << " but got Boolean instead";
							Toolbox::throwException(msgStream.str());
						}
					}
					else if (args[i]->IsNumber())
					{
						if (NumberProxy::isNumber(expectedArg->GetTypeName()))
						{
							// TODO
						}
						else
						{
							std::ostringstream msgStream;
							msgStream << "Error while validating arg " << i << ": Expected " << expectedArg->GetTypeNormalizedName() << " but got Number instead";
							Toolbox::throwException(msgStream.str());
						}
					}
					else if (args[i]->IsString())
					{
						if (StringProxy::isString(expectedArg->GetTypeName()))
						{
							// TODO
						}
						else
						{
							std::ostringstream msgStream;
							msgStream << "Error while validating arg " << i << ": Expected " << expectedArg->GetTypeNormalizedName() << " but got String instead";
							Toolbox::throwException(msgStream.str());
						}
					}
					else
					{
						std::ostringstream msgStream;
						msgStream << "Error while validating arg " << i << ": v8::Value at " << &(*(*args[i])) << "is neither a v8::Object nor a v8::Primitive";
						Toolbox::throwException(msgStream.str());
					}
				}
			}
		}
		else
		{
			std::ostringstream msgStream;
			msgStream << method.GetName();

			if (method.GetNargs() == method.GetNargsOpt())
			{
				msgStream << " takes exactly " << method.GetNargs();
			}
			else if (args.Length() < method.GetNargs())
			{
				msgStream << " takes at least " << method.GetNargs();
			}
			else if (args.Length() > method.GetNargsOpt())
			{
				msgStream << " takes at most " << method.GetNargsOpt();
			}

			msgStream << " arguments (" << args.Length() << " given)";
			Toolbox::throwException(msgStream.str());
		}

		return validatedArgs;
	}

	char* argToChar(v8::Local<v8::Value> originalArg) {
		v8::String::Utf8Value string(originalArg->ToString());
		char *str = (char *) malloc(string.length() + 1);
		strcpy(str, *string);
		return str;
	}

	double getDoubleFromArg(v8::Local<v8::Value> originalArg) {
		if(originalArg->IsNumber()) {
			return v8::Local<v8::Number>::Cast(originalArg)->Value();
		} else if(originalArg->IsNumberObject()) {
			return v8::Local<v8::NumberObject>::Cast(originalArg)->ValueOf();
		} else {
			return -1;
		}
	}

	double* argToDouble(v8::Local<v8::Value> originalArg) {
		double *doubleValue = (double*)malloc(sizeof(double));
		*doubleValue = getDoubleFromArg(originalArg);
		return doubleValue;
	}

	int* argToInt(v8::Local<v8::Value> originalArg) {
		int *intValue = (int *)malloc(sizeof(int));
		*intValue = (int)getDoubleFromArg(originalArg);
		return intValue;
	}

	bool* argToBool(v8::Local<v8::Value> originalArg) {
		bool* boolValue = (bool*)malloc(sizeof(bool));
		if(originalArg->IsBoolean()) {
			*boolValue = v8::Local<v8::Boolean>::Cast(originalArg)->Value();
		} else {
			*boolValue = v8::Local<v8::BooleanObject>::Cast(originalArg)->ValueOf();
		}
		return boolValue;
	}

	TString* argToTString(v8::Local<v8::Value> originalArg) {
		v8::String::Utf8Value string(originalArg->ToString());
		return new TString(*string);
	}

	void* FunctionProxy::bufferParam(TMethodArg* arg, v8::Local<v8::Value> originalArg) {
		std::map<std::string, mappedTypes>::iterator iterator = typeMap.find(std::string(arg->GetTypeName()));
		if(iterator == typeMap.end()) {
			Toolbox::throwException(std::string("bufferParam does not know how to handle ") + arg->GetTypeName());
			return nullptr;
		}
		switch(iterator->second) {
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

	void FunctionProxy::prepareCall(const  v8::Local<v8::Array>& args) {
		CallFunc_t* callFunc = (CallFunc_t*)getCallFunc(scope, function);
		if(!callFunc) {
			//TODO Handle this, should not segfault (maybe throw something...)
		}
		this->facePtr = gCling->CallFunc_IFacePtr( callFunc );

		buf = std::vector<void*>( args->Length() );
		for(int i = 0; i < (int)args->Length(); i++) {
			void** bufEl = (void**)malloc(sizeof(void*));
			*bufEl = bufferParam((TMethodArg*)(function->GetListOfMethodArgs()->At(i)), args->Get(i));
			buf[i] = bufEl;
		}
	}

	ObjectProxy* FunctionProxy::call()
	{
		void *self = nullptr; //TODO?
		void *result = nullptr; //TODO?
		switch(facePtr.fKind) {
		case (TInterpreter::CallFuncIFacePtr_t::kGeneric):

			facePtr.fGeneric((selfAddress == nullptr)?self:*(void**)selfAddress, buf.size(), buf.data(), &result);
			break;
		default:
			v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Jonas was too lazy to implement this..."));
		}

		for(int i = 0; i < (int)buf.size(); i++) {
			free(*((void**)buf[i]));
			free((void*)buf[i]);
		}

		//if(result != nullptr) {
			PointerInfo mode((void*)&result, function->GetReturnTypeName());
			ObjectProxy* proxy = ObjectProxyFactory::determineProxy(mode, TClassRef());

			if(proxy) {
				proxy->backup();
				return proxy;
			}
		//}

		return nullptr;
	}

	FunctionProxy* FunctionProxy::clone() {
		FunctionProxy *p = new FunctionProxy(address, *(rootJS::FunctionInfo*)type, function, scope);
		p->buf = buf;
		p->facePtr = facePtr;
		p->selfAddress = selfAddress;
		return p;
	}

	bool FunctionProxy::determineOverload(const v8::Local<v8::Array>& info) {
		TFunction* overloadedFunction = FunctionProxyFactory::determineFunction(function->GetName(), scope.GetClass(), info);
		if(overloadedFunction == nullptr) {
			return false;
		}
		function = overloadedFunction;
		return true;
	}


	/*
	// TODO
	bool FunctionProxy::processCall(TFunction* method, void* args, void* self, void* result)
	{
	}

	void* FunctionProxy::callConstructor(TFunction* method, TClassRef type, void* args)
	{
	}

	void FunctionProxy::callDestructor(TClassRef type, void* self)
	{
	}

	void* FunctionProxy::callObject(TFunction* method, void* self, void* args, TClassRef resType)
	{
	}

	template <typename T>
	T FunctionProxy::callPrimitive(TFunction* method, void* self, void* args)
	{
	}*/
}
