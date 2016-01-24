#ifndef FUNCTION_PROXY_FACTORY_H
#define FUNCTION_PROXY_FACTORY_H

#include "FunctionProxy.h"
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>

namepsace RootJS {
	class FunctionProxyFactory { 
	private:
		FunctionProxyFactory(void);
	public:
		static FunctionProxy& createFunctionProxy(TFunction function, TClassRef scope);
		static FunctionProxy& fromArgs(std::string name, TClassRef scope, v8::FunctionCallbackInfo args);
	}
}

#endif //FUNCTION_PROXY_FACTORY_H
