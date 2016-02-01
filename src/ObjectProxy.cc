#include "ObjectProxy.h"

#include <TObject.h>
#include <TGlobal.h>

namespace rootJS {

	ObjectProxy::ObjectProxy(MetaInfo &type, TClassRef scope)
		: Proxy(type, scope) {

	}

	ObjectProxy::~ObjectProxy() {
		delete currentmode;
		currentmode = nullptr;
	}

	const char* ObjectProxy::getTypeName() {
		return type->getTypeName();
	}

	MetaInfo *ObjectProxy::getTypeInfo() {
		return type;
	}

	Long_t ObjectProxy::getOffset() {
		return type->getOffset();
	}

	void ObjectProxy::set(ObjectProxy &value) {
		// TODO: validate type equality
		//address = value.getAddress();
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
		return type->isGlobal();
	}

	bool ObjectProxy::isConst() {
		return type->isConst();
	}

	bool ObjectProxy::isStatic() {
		return type->isStatic(); // TODO
	}

	void ObjectProxy::setValue(v8::Local<v8::Value> value) {
		return;
	}
}
