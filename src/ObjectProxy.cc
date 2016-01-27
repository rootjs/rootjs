#include "ObjectProxy.h"

#include <TObject.h>
#include <TGlobal.h>

namespace rootJS {

	ObjectProxy::ObjectProxy(const TDataMember &type, TClassRef scope)
		: Proxy(nullptr, type, scope) {
		currentmode = new MemberMode(type, nullptr);
	}

	ObjectProxy::ObjectProxy(const TGlobal &type, TClassRef scope)
		: Proxy(nullptr, type, scope) {
		currentmode = new GlobalMode(type);

	}

	ObjectProxy::~ObjectProxy() {
		delete currentmode;
		currentmode = nullptr;
	}

	const char* ObjectProxy::getTypeName() {
		return currentmode->getTypeName();
	}

	ProxyMode *ObjectProxy::getTypeInfo() {
		return currentmode;
	}

	Long_t ObjectProxy::getOffset() {
		return currentmode->getOffset();
	}

	void ObjectProxy::set(ObjectProxy &value) {
		// TODO: validate type equality
		address = value.getAddress();
	}

	void *ObjectProxy::getAddress() {
		return currentmode->getAddress();
	}

	v8::Local<v8::Value> ObjectProxy::get() {
		// objects just return their holder - i.e the proxy member
		return getProxy();
	}

	void ObjectProxy::setProxy(v8::Local<v8::Object> proxy) {
		this->proxy.Reset(v8::Isolate::GetCurrent(), proxy);
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
		return currentmode->isGlobal();
	}

	bool ObjectProxy::isConst() {
		return currentmode->isConst();
	}

	bool ObjectProxy::isStatic() {
		return currentmode->isStatic(); // TODO
	}

	void ObjectProxy::setValue(v8::Local<v8::Value> value) {
		return;
	}
}
