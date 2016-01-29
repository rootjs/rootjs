#include "FunctionProxyFactory.h"

#include "FunctionProxy.h"

#include <iostream>
#include <vector>
#include <map>

#include <v8.h>
#include <TROOT.h>
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>
#include <TMethodArg.h>

namespace rootJS {

	std::map<std::string, v8BasicTypes> FunctionProxyFactory::basicTypeMap = {
		{"char", v8BasicTypes::STRING}
	};

	FunctionProxyFactory::FunctionProxyFactory() {
		//TODO
	}

	FunctionProxy* FunctionProxyFactory::createFunctionProxy(TFunction *function, TClassRef scope) {
		return new FunctionProxy(FunctionProxy::getCallFunc(function), *function, scope);
	}

	FunctionProxy* FunctionProxyFactory::fromArgs(std::string name, TClassRef scope, v8::FunctionCallbackInfo<v8::Value> args) {
		std::vector<TFunction*> validFuncs;
		TFunction *callableFunction = nullptr;
		if(scope.GetClass() == nullptr) { // Global function has been called
			TCollection *globals = gROOT->GetListOfGlobalFunctions(kTRUE);
			TFunction *func;

			TIter next(globals);
			while((func = (TFunction*)next())) {
				if(strcmp(func->GetName(), name.c_str()) == 0) {
					validFuncs.push_back(func);
				}
			}
		} else {
			validFuncs = FunctionProxy::getMethodsFromName(scope, name);
		}

		for(TFunction* value: validFuncs) {
			if(value->GetNargs() != args.Length()) {
				continue;
			}
			TList *funcArgs = value->GetListOfMethodArgs();
			bool argsMatch = true;
			for(int i = 0; i < args.Length(); i++) {
				if(!paramMatches(((TMethodArg*)funcArgs->At(i))->GetTypeName(), args[i])) {
					argsMatch = false;
					break;
				}
			}
			if(argsMatch) {
				callableFunction = value;
				break;
			}
		}
		if(callableFunction) {
			return createFunctionProxy(callableFunction, scope);
		}

		return nullptr;
	}

	void* FunctionProxyFactory::createInstance(TClassRef &type, v8::Local<v8::Array> args) {
		// TODO
		return nullptr;
	}

	bool FunctionProxyFactory::paramMatches(const char* type, v8::Local<v8::Value> object) {
		std::map<std::string, v8BasicTypes>::iterator it
		    = basicTypeMap.find(std::string(type));
		if(it != basicTypeMap.end()) {
			switch(it->second) {
			case v8BasicTypes::STRING:
				return object->IsString();
			default:
				v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Jonas was too lazy to implement this..."));
				return false;
			}
		} else {
			v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Jonas was too lazy to implement this as well..."));
			return false;
		}
	}

}
