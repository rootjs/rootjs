#include "FunctionProxyFactory.h"

#include "FunctionProxy.h"
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>

namepsace RootJS {
	FunctionProxyFactory { 
		FunctionProxyFactory::FunctionProxyFactory() {
			//TODO
		}
		static FunctionProxy& createFunctionProxy(TFunction function, TClassRef scope) {
			return NULL //TODO
		}
		static FunctionProxy& fromArgs(std::string name, TClassRef scope, v8::FunctionCallbackInfo args) {
			return NULL //TODO
		}
	}
}
