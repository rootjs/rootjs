#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

#include <v8.h>
#include <map>
#include <string>

#include "ObjectProxy.h"

namespace RootJS {
	class CallbackHandler {
	private:
		static std::map<std::string, ObjectProxy*> globalProxyMap;
	public:
		static void setGlobalProxy(const std::string &name, ObjectProxy* proxy);
		static void globalGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
		static void globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	};
}

#endif /* end of include guard: CALLBACK_HANDLER_H */
