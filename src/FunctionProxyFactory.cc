#include "FunctionProxyFactory.h"

#include "FunctionProxy.h"
#include <v8.h>
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>

namespace RootJS {

	FunctionProxyFactory::FunctionProxyFactory() {
		//TODO
	}

	FunctionProxy* FunctionProxyFactory::createFunctionProxy(TFunction function, TClassRef scope) {
		return nullptr; //TODO
	}

	FunctionProxy* FunctionProxyFactory::fromArgs(std::string name, TClassRef scope, v8::FunctionCallbackInfo<v8::Value> args) {
		return nullptr; //TODO
	}

	void* FunctionProxyFactory::createInstance(TClassRef &type, v8::Local<v8::Array> args) {
		// TODO
		return nullptr;
	}

}
