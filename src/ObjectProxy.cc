#include "ObjectProxy.h"

#include "PointerInfo.h"

#include <TObject.h>
#include <TGlobal.h>

namespace rootJS {

	ObjectProxy::ObjectProxy(MetaInfo &info, TClass *scope) : Proxy(info, scope) {

	}

	ObjectProxy::~ObjectProxy()
	{
	}

	const char* ObjectProxy::getTypeName() {
		return getTypeInfo()->getTypeName();
	}

	Long_t ObjectProxy::getOffset() {
		return getTypeInfo()->getOffset();
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
		return false;
	}

	bool ObjectProxy::isGlobal() {
		return getTypeInfo()->isGlobal();
	}

	bool ObjectProxy::isConst() {
		return getTypeInfo()->isConst();
	}

	bool ObjectProxy::isStatic() {
		return getTypeInfo()->isStatic();
	}

	void ObjectProxy::setValue(v8::Local<v8::Value> value) {
		return;
	}

	void ObjectProxy::backup() {
		//Nothing to do here, PointerInfo should only be used for heap pointers
	}
}
