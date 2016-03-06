#ifndef FUNCTION_PROXY_H
#define FUNCTION_PROXY_H

#include "Proxy.h"
#include "ObjectProxy.h"
#include "ObjectProxyBuilder.h"

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
	enum class mappedTypes {
		CHAR, CSTR, INT, DOUBLE, BOOL, UINT, LDOUBLE, SHORT, USHORT, UCHAR,
		LONG, LLONG, ULONG, ULLONG, FLOAT
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
			FunctionProxy(FunctionInfo &info, TFunction *function, TClass *scope);

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
			void call(void *self, ObjectProxyBuilder &builder, bool isConstructorCall = false);

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
			 * Check if this proxy encapsulates a constant.
			 *
			 * @return true if this ProxyObject encapsulates a constant
			 */
			virtual bool isConst()
			{
				return info->isConst();
			};

			/**
			 * Check if this proxy encapsulates a global.
			 *
			 * @return true if this ProxyObject encapsulates a global
			 */
			virtual bool isGlobal()
			{
				return info->isGlobal();
			};

			/**
			 * Check if this proxy encapsulates a static.
			 *
			 * @return true if this ProxyObject encapsulates a static
			 */
			virtual bool isStatic()
			{
				return info->isStatic();
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
			static std::vector<void*> pointerAlignmentBuffer;

			TInterpreter::CallFuncIFacePtr_t facePtr;
			TFunction* function;
			std::vector<void*> buf;
			std::vector<bool> bufCopied;

			static void *alignPointerCount(void *param, int derefCount);


			static void* bufferParam(TMethodArg* arg, v8::Local<v8::Value> originalArg, bool &copied);

			static char*    			argToCstr   	(v8::Local<v8::Value> originalArg);
			static char*    			argToChar   	(v8::Local<v8::Value> originalArg);
			static double*  			argToDouble 	(v8::Local<v8::Value> originalArg);
			static int*    		 		argToInt    	(v8::Local<v8::Value> originalArg);
			static float*   			argToFloat 		(v8::Local<v8::Value> originalArg);
			static bool*    			argToBool   	(v8::Local<v8::Value> originalArg);
			static long double*			argToLDouble   	(v8::Local<v8::Value> originalArg);
			static long long*			argToLLong   	(v8::Local<v8::Value> originalArg);
			static long*				argToLong   	(v8::Local<v8::Value> originalArg);
			static short*				argToShort   	(v8::Local<v8::Value> originalArg);
			static unsigned short*		argToUShort   	(v8::Local<v8::Value> originalArg);
			static unsigned char*		argToUChar   	(v8::Local<v8::Value> originalArg);
			static unsigned int*		argToUInt   	(v8::Local<v8::Value> originalArg);
			static unsigned long long*	argToULLong   	(v8::Local<v8::Value> originalArg);
			static unsigned long*		argToULong   	(v8::Local<v8::Value> originalArg);
			static void* 				argToObj		(v8::Local<v8::Value> originalArg, int derefCount);
			static TString* 			argToTString	(v8::Local<v8::Value> originalArg);

			static double   getDoubleFromArg(v8::Local<v8::Value> originalArg);
	};
}

#endif
