#ifndef FUNCTION_PROXY_H
#define FUNCTION_PROXY_H

#include "Proxy.h"
#include "ObjectProxy.h"

#include <map>
#include <string>
#include <vector>

#include <v8.h>

#include <TClassRef.h>
#include <TFunction.h>

namespace RootJS {
	/**
	 * Represents a ROOT callable and provides functionality to invoke those callables.
	 * Also acts as a static cache for already created FunctionProxy objects.
	 */
	class FunctionProxy: public Proxy {
	public:
		/**
		 * Get a pointer to a CallFunc object, which encapsulates the ROOT function in memory.
		 *
		 * @param method the callable whose CallFunc object shall be returned
		 * @return a pointer to the CallFunc object provided by cling
		 */
		static CallFunc_t* getCallFunc(TFunction* method);

		/**
		 * Get all methods of the specified class with the specified name.
		 *
		 * @param scope reference to the class which is checked for methods with the specified name
		 * @param name name of the overloaded methods which shall be returned
		 * @return a vector of methods that match the specified name
		 */
		static std::vector<TFunction*> getMethodsFromName(TClassRef scope, std::string name);

		/**
		 * Create a new FunctionProxy.
		 *
		 * @param address memory address of the proxied function
		 * @param function the function's reflection object
		 * @param scope the class that the function belongs to
		 */
		FunctionProxy(void* address, TFunction function, TClassRef scope);

		/**
		 * Get the wrapped function's TFunction object which contains the meta data of its corresponding function
		 *
		 * @return the TFunction object that contains the function's reflection data
		 */
		const TFunction& getType();

		/**
		 * Check whether the arguments encapsulated in the FunctionCallbackInfo
		 * are valid arguments to the function. The parameters are then wrapped
		 * in proxies so they can be<char> v; used by the call method.
		 *
		 * @param args contains the arguments which shall be validated
		 * @return an array of proxies for the validated arguments
		 */
		std::vector<ObjectProxy*> validateArgs(v8::FunctionCallbackInfo<v8::Value> args);

		/**
		 * Invokes the proxied function.
		 *
		 * @param args the arguments for the function call.
		 * @return the function's return value encasulated in an ObjectProxy
		 */
		ObjectProxy call(ObjectProxy args[]) const;

	private:
		static bool processCall(TFunction* method, void* args, void* self, void* result);

		static void* callConstructor(TFunction* method, TClassRef type, void* args);

		static void callDestructor(TClassRef type, void* self);

		static void* callObject(TFunction* method, void* self, void* args, TClassRef resType);

		template <typename T>
		static T callPrimitive(TFunction* method, void* self, void* args);

		static std::map<TFunction*, CallFunc_t*> functions;
	};
}

#endif // FUNCTION_PROXY_H
