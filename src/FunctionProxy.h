#ifndef FUNCTION_PROXY_H
#define FUNCTION_PROXY_H

#include "Proxy.h"
#include "ObjectProxy.h"

#include <map>
#include <string>
#include <vector>

#include <v8.h>

#include <TClassRef.h>
#include <TInterpreter.h>
#include <TFunction.h>
#include <TMethodArg.h>
#include "FunctionInfo.h"

namespace rootJS
{
	enum class mappedTypes
	{
	    CHAR, INT, DOUBLE, BOOL, TSTRING
	};

	/**
	 * Represents a ROOT callable and provides functionality to invoke those callables.
	 * Also acts as a static cache for already created FunctionProxy objects.
	 */
	class FunctionProxy: public Proxy
	{
		public:
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
			FunctionProxy(void *address, FunctionInfo &mode, TFunction *function, TClass *scope);

			/**
			 * Makes a clone of the current FunctionProxy
			 *
			 * @return a pointer to the clone
			 */
			FunctionProxy* clone();

			/**
			 * Get a pointer to a CallFunc object, which encapsulates the ROOT function in memory.
			 *
			 * @param method the callable whose CallFunc object shall be returned
			 * @return a pointer to the CallFunc object provided by cling
			 */
			static CallFunc_t* getCallFunc(const TClassRef& klass, TFunction* method);

			void prepareCall(const v8::Local<v8::Array>& args);

			/**
			 * Invokes the proxied function.
			 *
			 * @param args the arguments for the function call.
			 * @return the function's return value encasulated in an ObjectProxy
			 */
			ObjectProxy* call(bool isConstructorCall = false);

			/**
			 * TODO: verify
			 *
			 * Check whether the arguments encapsulated in the FunctionCallbackInfo
			 * are valid arguments to the function. The parameters are then wrapped
			 * in proxies so they can be<char> v; used by the call method.
			 *
			 * @param args contains the arguments which shall be validated
			 * @return an array of proxies for the validated arguments
			std::vector<ObjectProxy*> validateArgs(v8::FunctionCallbackInfo<v8::Value> args);

			**
			 * Determines which overloaded function is wanted
			 * @param info The info of the overloaded function
			 * @return true if the overloaded function is found
			bool determineOverload(const v8::Local<v8::Array>& info);
			 */

			/**
			 * Sets the address of the function
			 *
			 * @param addr
			 *              The address the function will be set to
			 */
			void setSelfAddress(void* addr)
			{
				selfAddress = addr;
			}

			/**
			 * Check if this proxy encapsulates a constant.
			 *
			 * @return true if this ProxyObject encapsulates a constant
			 */
			virtual bool isConst()
			{
				return true;
			};

			/**
			 * Check if this proxy encapsulates a global.
			 *
			 * @return true if this ProxyObject encapsulates a global
			 */
			virtual bool isGlobal()
			{
				return true; /*TODO*/
			};

			/**
			 * Check if this proxy encapsulates a static.
			 *
			 * @return true if this ProxyObject encapsulates a static
			 */
			virtual bool isStatic()
			{
				return true; /*TODO*/
			};

			/**
			 * Check if this proxy encapsulates a template.
			 *
			 * @return true if this ProxyObject encapsulates a template
			 */
			virtual bool isTemplate()
			{
				return false; /*TODO*/
			};

		private:
			static std::map<TFunction*, CallFunc_t*> functions;
			static std::map<std::string, mappedTypes> typeMap;

			TInterpreter::CallFuncIFacePtr_t facePtr;
			TFunction* function;
			std::vector<void*> buf;

			void *address;
			void *selfAddress = 0;

			static void* bufferParam(TMethodArg* arg, v8::Local<v8::Value> originalArg);

			static char*    argToChar   (v8::Local<v8::Value> originalArg);
			static double*  argToDouble (v8::Local<v8::Value> originalArg);
			static int*     argToInt    (v8::Local<v8::Value> originalArg);
			static bool*    argToBool   (v8::Local<v8::Value> originalArg);
			static TString* argToTString(v8::Local<v8::Value> originalArg);

			static double   getDoubleFromArg(v8::Local<v8::Value> originalArg);

			/*
			static bool processCall(TFunction* method, void* args, void* self, void* result);

			static void* callConstructor(TFunction* method, TClassRef type, void* args);

			static void callDestructor(TClassRef type, void* self);

			static void* callObject(TFunction* method, void* self, void* args, TClassRef resType);

			template <typename T>
			static T callPrimitive(TFunction* method, void* self, void* args);
			*/

	};
}

#endif
