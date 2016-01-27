#include "ObjectProxy.h"

#include <TObject.h>
#include <TGlobal.h>

rootJS::ObjectProxy::ObjectProxy(const TDataMember& type, TClassRef scope) :
	Proxy(nullptr, type, scope) {

}

rootJS::ObjectProxy::ObjectProxy(void *object, const TGlobal & type,
                                 TClassRef scope) :
	Proxy(object, type, scope) {

}

rootJS::ObjectProxy::~ObjectProxy() {

}

const TDataMember& rootJS::ObjectProxy::getType() {
	return dynamic_cast<const TDataMember&>(type);
}

void rootJS::ObjectProxy::set(ObjectProxy& value) {
	// TODO: validate type equality
	address = value.getAddress();
}

v8::Local<v8::Value> rootJS::ObjectProxy::get() {
	// objects just return their holder - i.e the proxy member
	return getProxy();
}

void rootJS::ObjectProxy::setProxy(v8::Local<v8::Object> proxy) {
	this->proxy.Reset(v8::Isolate::GetCurrent(), proxy);
}

v8::Local<v8::Object> rootJS::ObjectProxy::getProxy() {
	return v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), proxy);
}

bool rootJS::ObjectProxy::isPrimitive() {
	return false;
}

bool rootJS::ObjectProxy::isTemplate() {
	return false; //TODO
}

bool rootJS::ObjectProxy::isGlobal() {
	return false; //TODO
}

bool rootJS::ObjectProxy::isConst() {
	return false; //TODO
}

bool rootJS::ObjectProxy::isStatic() {
	return false; //TODO
}

void rootJS::ObjectProxy::setValue(v8::Local<v8::Value> value) {
	return;
}
