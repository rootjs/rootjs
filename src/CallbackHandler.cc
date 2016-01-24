#include "CallbackHandler.h"

#include <string>
#include <map>

namespace RootJS {

	std::map<std::string, ObjectProxy*> CallbackHandler::globalProxyMap;

	void CallbackHandler::setGlobalProxy(const std::string &name, ObjectProxy* proxy) {
		globalProxyMap[name] = proxy;
	}

	void CallbackHandler::globalGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
		ObjectProxy* proxy = globalProxyMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy) {
			info.GetReturnValue().Set(proxy->get());
		}
	}

	void CallbackHandler::globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
		ObjectProxy* proxy = globalProxyMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy) {
			proxy->setValue(value);
		}
	}
}
