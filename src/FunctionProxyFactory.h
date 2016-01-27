#ifndef FUNCTION_PROXY_FACTORY_H
#define FUNCTION_PROXY_FACTORY_H

#include "FunctionProxy.h"

#include <vector>

#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>

namespace RootJS {
	class FunctionProxyFactory {
	private:
		FunctionProxyFactory(void);
	public:
		static FunctionProxy* createFunctionProxy(TFunction function, TClassRef scope);
		static FunctionProxy* fromArgs(std::string name, TClassRef scope, v8::FunctionCallbackInfo<v8::Value> args);

		/**
		 * Create a new instance of the specified type using the constructor suitable to the supplied arguments.
		 *
		 * @param type
		 * 			the type of the instance that will be created
		 *
		 * @param args
		 * 			the arguments to call the constructor with
		 *
		 * @return the address to the newly created instance of the specified type or nullptr if no suitable constructor was found
		 */
		static void* createInstance(TClassRef &type, v8::Local<v8::Array> args);
	};
}

#endif //FUNCTION_PROXY_FACTORY_H
