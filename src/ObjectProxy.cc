#include "ObjectProxy.h"

RootJS::ObjectProxy::ObjectProxy(TDataMember& type, TClassRef scope): Proxy(nullptr, type, scope){

}

RootJS::ObjectProxy::~ObjectProxy() {

}

TDataMember& RootJS::ObjectProxy::getType() {
	return dynamic_cast<TDataMember&>(type);
}

void RootJS::ObjectProxy::set(ObjectProxy & value) {
	// TODO: validate type equality
	address = value.getAddress();
}

v8::Local<v8::Value> RootJS::ObjectProxy::get() {
	// objects just return their holder - i.e the proxy member
	return getProxy();
}

void RootJS::ObjectProxy::setProxy(v8::Local<v8::Object> proxy) {
	this->proxy.Reset(v8::Isolate::GetCurrent(), proxy);
}

v8::Local<v8::Object> RootJS::ObjectProxy::getProxy() {
	return v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), proxy);
}

bool RootJS::ObjectProxy::isPrimitive() {
	return false;
}
