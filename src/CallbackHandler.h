#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

#include <v8.h>
#include <map>
#include <string>

#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "FunctionProxyFactory.h"

#include "Rtypes.h"
#include "TClass.h"
#include "TClassRef.h"
#include "TClassTable.h"

namespace rootJS
{
	class CallbackHandler
	{
	public:

		/**
		 * 	TODO: fill in description
		 *
		 *	@param name
		 *
		 *
		 *	@param proxy
		 *
		 *
		 */
		static void setGlobalProxy(const std::string &name, ObjectProxy* proxy);

		/**
		 * 	TODO: fill in description
		 *
		 *	@param property
		 *
		 *
		 *	@param info
		 *
		 *
		 */
		static void globalGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

		/**
		 * 	TODO: fill in description
		 *
		 *	@param property
		 *
		 *
		 *	@param value
		 *
		 *
		 *	@param info
		 *
		 *
		 */
		static void globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

		/**
		 * 	This callback method may be invoked whenever a JavaScript prototype function of an encapsulated ROOT class was called.
		 *
		 * 	Based on the supplied arguments the suitable C++ constructor will be called.
		 * 	Then the newly created instance will be wrapped into an ObjectProxy.
		 * 	As result the ObjectProxy's corresponding JavaScript object will be returned to the Node.js application.
		 *
		 * 	In order to enable non blocking object creation one can supply a JavaScript callback function as last argument of the prototype function.
		 * 	After the asynchronous object creation is finished the supplied callback will be invoked for returning the generated JavasSript proxy.
		 *
		 *	@param info
		 *			the argument information given to this function call callback
		 *
		 */
		static void ctorCallback(const v8::FunctionCallbackInfo<v8::Value>& info);

		/**
		 * 	TODO: fill in description
		 *
		 *	@param property
		 *
		 *
		 *	@param info
		 *
		 *
		 */
		static void getterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);

		/**
		 * 	TODO: fill in description
		 *
		 *	@param property
		 *
		 *
		 *	@param value
		 *
		 *
		 *	@param info
		 *
		 *
		 */
		static void setterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);

		static void staticFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void globalFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args);


	private:
		static std::map<std::string, ObjectProxy*> globalProxyMap;

		static v8::Local<v8::Array> getInfoArgs(int beginIndex, int endIndex, const v8::FunctionCallbackInfo<v8::Value>& info);
	};
}

#endif
