#ifndef FUNCTION_PROXY_FACTORY_H
#define FUNCTION_PROXY_FACTORY_H

#include "FunctionProxy.h"

#include <vector>

#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>

namespace rootJS
{
	enum class v8BasicTypes
	{
	    BOOLEAN,
	    STRING,
	    NUMBER,
	    ARRAY,
	    OBJECT
	};

	class FunctionProxyFactory
	{
		private:
			static std::map<std::string, v8BasicTypes> basicTypeMap;

			static bool paramMatches(const char* type, v8::Local<v8::Value> object);

			FunctionProxyFactory(void);
		public:
			/**
			 * Create a new FunctionProxy of the given function
			 *
			 * @param function
			 * 			the function to be proxied
			 *
			 * @param scope
			 * 			the type of the instance that will be created
			 *
			 * @return the pointer to the newly created FunctionProxy
			 */
			static FunctionProxy* createFunctionProxy(TFunction *function, TClass *scope);
			/**
			 * Uses the parameters to determine which function is to be called up
			 *
			 * @param name
			 * 			the name of the function
			 *
			 * @param scope
			 * 			the type of the instance that will be created
			 *
			 * @param args
			 * 			the arguments of the function
			 *
			 * @return the pointer to the function which was determined
			 */
			static TFunction* determineFunction(std::string name, TClass *scope, const v8::Local<v8::Array> args);

			/**
			 * Determines which overloaded function should be called up
			 *
			 * @param name
			 * 			the name of the function
			 *
			 * @param scope
			 * 			the type of the instance that will be created
			 *
			 * @param args
			 * 			the arguments of the function
			 *
			 * @return the pointer to the function proxy of the overloaded function
			 */
			static FunctionProxy* fromArgs(std::string name, TClass *scope, v8::Local<v8::Array> args);

			/**
			 * Create a new instance of the specified type using the constructor suitable to the supplied arguments.
			 *
			 * @param name
			 * 			the name of the constructor function
			 *
			 * @param scope
			 * 			the type of the instance that will be created
			 *
			 * @param args
			 * 			the arguments to call the constructor with
			 *
			 * @return the address to the newly created instance of the specified type or nullptr if no suitable constructor was found
			 */
			static void* createInstance(std::string name, TClass *scope, v8::Local<v8::Array> args);
	};
}

#endif //FUNCTION_PROXY_FACTORY_H
