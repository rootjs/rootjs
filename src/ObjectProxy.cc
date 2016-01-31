#include "ObjectProxy.h"

namespace rootJS {

	ObjectProxy::ObjectProxy(void* address, MetaInfo *info, TClass* scope) : Proxy(address, scope), info(info) {

	}

	ObjectProxy::ObjectProxy(MetaInfo *info, TClass* scope) : ObjectProxy(nullptr, info, scope) {

	}

	ObjectProxy::~ObjectProxy() {
		delete info;
		info = nullptr;
	}

	MetaInfo* ObjectProxy::getMetaInfo() {
		return info;
	}

	void ObjectProxy::set(ObjectProxy &value) {
		// TODO: validate type equality
		address = value.getAddress();
	}

	void ObjectProxy::setValue(v8::Local<v8::Value> value) {
		// TODO remove
	}

	void *ObjectProxy::getAddress() {
		return address;
	}

	v8::Local<v8::Value> ObjectProxy::get() {
		// objects just return their holder - i.e the proxy member
		return getProxy();
	}

	void ObjectProxy::setProxy(v8::Local<v8::Object> proxy) {
		if(this->proxy.IsEmpty()) {
			this->proxy.Reset(v8::Isolate::GetCurrent(), proxy);
			/*
			this->proxy.SetWeak(this, dtorCallback);
			this->proxy.MarkIndependent();
			*/
		}
	}

	v8::Local<v8::Object> ObjectProxy::getProxy() {
		return v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), proxy);
	}

	bool ObjectProxy::isPrimitive() {
		return false;
	}

	bool ObjectProxy::isTemplate() {
		return false; // TODO
	}

	bool ObjectProxy::isGlobal() {
		return info->getProperty() & kIsStatic;
	}

	bool ObjectProxy::isConst() {
		return info->getProperty() & kIsConstant;
	}

	bool ObjectProxy::isStatic() {
		return info->getProperty() & kIsStatic;
	}
}
