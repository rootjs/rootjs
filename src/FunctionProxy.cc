#include "FunctionProxy.h"

#include "FunctionProxyFactory.h"
#include "BooleanProxy.h"
#include "NumberProxy.h"
#include "StringProxy.h"
#include "FunctionInfo.h"
#include "PointerInfo.h"
#include "Toolbox.h"
#include "Types.h"

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

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
#include <TClassTable.h>

namespace rootJS
{
	std::map<TFunction*, CallFunc_t*> FunctionProxy::functions;
	std::map<std::string, mappedTypes> FunctionProxy::typeMap = {
		{"int", mappedTypes::INT},
		{"unsigned int", mappedTypes::UINT},

		{"double", mappedTypes::DOUBLE},
		{"long double", mappedTypes::LDOUBLE},

		{"short", mappedTypes::SHORT},
		{"unsigned short", mappedTypes::USHORT},

		{"unsigned char", mappedTypes::UCHAR},

		{"long", mappedTypes::LONG},
		{"long long", mappedTypes::LLONG},

		{"unsigned long", mappedTypes::ULONG},
		{"unsigned long long", mappedTypes::ULLONG},

		{"float", mappedTypes::FLOAT},

		{"char", mappedTypes::CHAR},
		{"char*", mappedTypes::CSTR},

		{"bool", mappedTypes::BOOL},
		{"Bool_t", mappedTypes::BOOL},

		{"Double32_t", mappedTypes::DOUBLE},
		{"Float16_t", mappedTypes::FLOAT},
		{"Long64_t", mappedTypes::LLONG},
		{"ULong64_t", mappedTypes::ULLONG}
	};
	std::vector<void*> FunctionProxy::pointerAlignmentBuffer;

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
		if (iterator != FunctionProxy::functions.end()) {
			return iterator->second;
		} else {
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

			/*
			 * Workaround ;)
			 * TString* string = (TString*) gROOT->ProcessLineFast("new TString(\"test\");");
			 *	Toolbox::logInfo(std::string(string->Data()));
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

	void FunctionProxy::prepareCall(const  v8::Local<v8::Array>& args)
	{
		CallFunc_t* callFunc = (CallFunc_t*)info->getAddress();
		if(!callFunc) {
			//TODO Handle this, should not segfault (maybe throw something...)
		}
		this->facePtr = gCling->CallFunc_IFacePtr( callFunc );
		gCling->CallFunc_Delete(callFunc);

		buf = std::vector<void*>( args->Length() );
		bufCopied = std::vector<bool>( args->Length() );
		for(int i = 0; i < (int)args->Length(); i++) {
			void** bufEl = (void**)malloc(sizeof(void*));
			bool copied = false;
			*bufEl = bufferParam((TMethodArg*)(function->GetListOfMethodArgs()->At(i)), args->Get(i), copied);
			bufCopied[i] = copied;
			if(!copied) {
				buf[i] = *bufEl;
				free(bufEl);
			} else {
				buf[i] = bufEl;
			}
		}
	}

	void FunctionProxy::call(void *self, ObjectProxyBuilder &builder, bool isConstructorCall /* false */)
	{
		void *result = nullptr;
		void **resultPtr;
		resultPtr = &result;
		void *allocated = nullptr;

		std::string returnTypeName(function->GetReturnTypeName());
		int ptrDepth = std::count(returnTypeName.begin(), returnTypeName.end(), '*');
		std::string typeName = function->GetReturnTypeNormalizedName();


		if(!isConstructorCall) {
			DictFuncPtr_t dictFunc = gClassTable->GetDict(typeName.c_str());
			if(dictFunc) {
				TClass *klass = dictFunc();
				result = malloc(klass->Size());
				allocated = result;
				resultPtr = (void**)result;
			}
		}

		switch(facePtr.fKind) {
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

		for(int i = 0; i < (int)buf.size(); i++) {
			if(bufCopied[i]) {
				free(*((void**)buf[i]));
				free((void*)buf[i]);
			}
		}

		for(void *ptr: pointerAlignmentBuffer) {
			free(ptr);
		}
		pointerAlignmentBuffer = std::vector<void*>();

		PointerInfo mode(result, typeName.c_str(), ptrDepth);
		builder.setResultInfo(mode);

		DictFuncPtr_t dictFunc = gClassTable->GetDict(typeName.c_str());
		if(dictFunc != nullptr) {
			builder.setClass(dictFunc());
		}


		if(allocated) {
			builder.bindAllocatedMemory(allocated);
		}
	}

	void* FunctionProxy::bufferParam(TMethodArg* arg, v8::Local<v8::Value> originalArg, bool& copied)
	{
		std::string fullTypeName = std::string(arg->GetFullTypeName());
		TDataType* fullType = Types::getTypeByName(fullTypeName);
		TDataType* type = Types::getTypeByName(std::string(arg->GetTypeName()));
		int derefCount = std::count(fullTypeName.begin(), fullTypeName.end(), '*');
		if(type == nullptr || fullType == nullptr) {
			//might be an object...
			DictFuncPtr_t dictFunc = gClassTable->GetDict(arg->GetTypeName());
			if(dictFunc == nullptr) {
				throw std::invalid_argument(std::string("bufferParam does not know how to handle ") + arg->GetTypeName());
				return nullptr;
			}
			copied = false;
			fullTypeName = std::string(arg->GetFullTypeName());

			return argToObj(originalArg, derefCount);
		}

		TString typeName = fullType->GetTypeName();
		std::string stdTypeName(typeName.Data());
		std::map<std::string, mappedTypes>::iterator iterator = typeMap.find(stdTypeName);
		if(iterator == typeMap.end()) {
			typeName = type->GetTypeName();
			stdTypeName = typeName.Data();
			iterator = typeMap.find(stdTypeName);
			if(iterator == typeMap.end()) {
				//Might be an object
				throw std::invalid_argument(std::string("bufferParam does not know how to handle ") + stdTypeName);
				return nullptr;
			}
		}

		copied = false;
		void *result = nullptr;
		switch(iterator->second) {
		case mappedTypes::CSTR:
			copied = true;
			result = argToCstr(originalArg);
			derefCount--;
			break;
		case mappedTypes::CHAR:
			result = argToChar(originalArg);
			break;
		case mappedTypes::INT:
			result = argToInt(originalArg);
			break;
		case mappedTypes::DOUBLE:
			result = argToDouble(originalArg);
			break;
		case mappedTypes::BOOL:
			result = argToBool(originalArg);
			break;
		case mappedTypes::FLOAT:
			result = argToFloat(originalArg);
			break;
		case mappedTypes::LDOUBLE:
			result = argToLDouble(originalArg);
			break;
		case mappedTypes::LLONG:
			result = argToLLong(originalArg);
			break;
		case mappedTypes::LONG:
			result = argToLong(originalArg);
			break;
		case mappedTypes::SHORT:
			result = argToShort(originalArg);
			break;
		case mappedTypes::UCHAR:
			result = argToUChar(originalArg);
			break;
		case mappedTypes::UINT:
			result = argToUInt(originalArg);
			break;
		case mappedTypes::ULLONG:
			result = argToULLong(originalArg);
			break;
		case mappedTypes::ULONG:
			result = argToULong(originalArg);
			break;
		case mappedTypes::USHORT:
			result = argToUShort(originalArg);
			break;
		}

		return alignPointerCount(result, derefCount);
	}

	char* FunctionProxy::argToCstr(v8::Local<v8::Value> originalArg)
	{
		v8::String::Utf8Value string(originalArg->ToString());
		char *str = (char *) malloc(string.length() + 1);
		strcpy(str, *string);
		return str;
	}

	char* FunctionProxy::argToChar(v8::Local<v8::Value> originalArg)
	{
		v8::String::Utf8Value string(originalArg->ToString());
		std::string stdStr(*string);
		char *str = (char *) malloc(sizeof(char));
		if(stdStr.length() < 1) {
			*str = '\0';
		} else {
			*str = stdStr[0];
		}
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

	float* FunctionProxy::argToFloat(v8::Local<v8::Value> originalArg)
	{
		float *intValue = (float *)malloc(sizeof(float));
		*intValue = (float)getDoubleFromArg(originalArg);
		return intValue;
	}

	long double* FunctionProxy::argToLDouble(v8::Local<v8::Value> originalArg)
	{
		long double *intValue = (long double *)malloc(sizeof(long double));
		*intValue = (long double)getDoubleFromArg(originalArg);
		return intValue;
	}

	long long* FunctionProxy::argToLLong(v8::Local<v8::Value> originalArg)
	{
		long long *intValue = (long long *)malloc(sizeof(long long));
		*intValue = (long long)getDoubleFromArg(originalArg);
		return intValue;
	}

	long* FunctionProxy::argToLong(v8::Local<v8::Value> originalArg)
	{
		long *intValue = (long *)malloc(sizeof(long));
		*intValue = (long)getDoubleFromArg(originalArg);
		return intValue;
	}

	short* FunctionProxy::argToShort(v8::Local<v8::Value> originalArg)
	{
		short *intValue = (short *)malloc(sizeof(short));
		*intValue = (short)getDoubleFromArg(originalArg);
		return intValue;
	}

	unsigned short* FunctionProxy::argToUShort(v8::Local<v8::Value> originalArg)
	{
		unsigned short *intValue = (unsigned short *)malloc(sizeof(unsigned short));
		*intValue = (unsigned short)getDoubleFromArg(originalArg);
		return intValue;
	}

	unsigned char* FunctionProxy::argToUChar(v8::Local<v8::Value> originalArg)
	{
		unsigned char *intValue = (unsigned char *)malloc(sizeof(unsigned char));
		*intValue = (unsigned char)getDoubleFromArg(originalArg);
		return intValue;
	}

	unsigned int* FunctionProxy::argToUInt(v8::Local<v8::Value> originalArg)
	{
		unsigned int *intValue = (unsigned int *)malloc(sizeof(unsigned int));
		*intValue = (unsigned int)getDoubleFromArg(originalArg);
		return intValue;
	}

	unsigned long long* FunctionProxy::argToULLong(v8::Local<v8::Value> originalArg)
	{
		unsigned long long *intValue = (unsigned long long *)malloc(sizeof(unsigned long long));
		*intValue = (unsigned long long)getDoubleFromArg(originalArg);
		return intValue;
	}

	unsigned long* FunctionProxy::argToULong(v8::Local<v8::Value> originalArg)
	{
		unsigned long *intValue = (unsigned long *)malloc(sizeof(unsigned long));
		*intValue = (unsigned long)getDoubleFromArg(originalArg);
		return intValue;
	}


	bool* FunctionProxy::argToBool(v8::Local<v8::Value> originalArg)
	{
		bool* boolValue = (bool*)malloc(sizeof(bool));
		if(originalArg->IsBoolean()) {
			*boolValue = v8::Local<v8::Boolean>::Cast(originalArg)->Value();
		} else {
			*boolValue = v8::Local<v8::BooleanObject>::Cast(originalArg)->ValueOf();
		}
		return boolValue;
	}

	TString* FunctionProxy::argToTString(v8::Local<v8::Value> originalArg)
	{
		v8::String::Utf8Value string(originalArg->ToString());
		return new TString(*string);
	}

	void* FunctionProxy::argToObj(v8::Local<v8::Value> originalArg, int derefCount)
	{
		v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(originalArg);
		if(obj->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
		{
			Toolbox::throwException("Unexpected internal field count.");
			return nullptr;
		}

		ObjectProxy *proxy = (ObjectProxy*)obj->GetAlignedPointerFromInternalField(Toolbox::InternalFieldData::ObjectProxyPtr);

		void *result = proxy->getAddress();
		return alignPointerCount(result, derefCount - 1);
	}


	double FunctionProxy::getDoubleFromArg(v8::Local<v8::Value> originalArg)
	{
		if(originalArg->IsNumber()) {
			return v8::Local<v8::Number>::Cast(originalArg)->Value();
		} else if(originalArg->IsNumberObject()) {
			return v8::Local<v8::NumberObject>::Cast(originalArg)->ValueOf();
		} else {
			return -1;
		}
	}

	void *FunctionProxy::alignPointerCount(void *param, int derefCount) {
		for(; derefCount < 0; derefCount++) {
			param = *(void**)param;
		}
		for(; derefCount > 0; derefCount--) {
			void **paramBuffer = (void**)malloc(sizeof(void*));
			*paramBuffer = (void*)&param;
			param = (void*)paramBuffer;
			pointerAlignmentBuffer.push_back(paramBuffer);
		}
		return param;
	}

}
