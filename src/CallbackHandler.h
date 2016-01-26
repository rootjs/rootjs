#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

#include <v8.h>
#include <map>
#include <string>

#include "ObjectProxy.h"

namespace RootJS
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
		 * 	TODO: fill in description
		 *
		 *	@param args
		 *
		 *
		 */
		static void ctorCallback(const v8::FunctionCallbackInfo<v8::Value>& args);

	private:
		static std::map<std::string, ObjectProxy*> globalProxyMap;
	};
}

#endif
