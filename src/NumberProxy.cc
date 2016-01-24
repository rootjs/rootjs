#include "NumberProxy.h"
#include <iostream>
#include <v8.h>

namespace RootJS {

	NumberProxy::NumberProxy(const TDataMember& type, TClassRef scope): PrimitiveProxy(type, scope) {

	}

	NumberProxy::NumberProxy(void * object, const TGlobal & type, TClassRef scope): PrimitiveProxy(object, type, scope) {

	}

	v8::Local<v8::Value> NumberProxy::get() {
		if(getAddress()) {
			return v8::NumberObject::New(v8::Isolate::GetCurrent(), castToDouble(getAddress()));
		}
		return getProxy();
	}

	Double_t NumberProxy::castToDouble(void *ptr) {
		switch(numberType) {
			case NumberType::INT_T: return *((Int_t*)getAddress());
			case NumberType::DOUBLE_T: return *((Double_t*)getAddress());
		}
	}

	ObjectProxy* NumberProxy::intConstruct(const TDataMember& type, TClassRef scope) {
		NumberProxy* proxy = new NumberProxy(type, scope);
		proxy->numberType = NumberType::INT_T;
		return proxy;
	}

	ObjectProxy* NumberProxy::intConstruct(void *address, const TGlobal& type, TClassRef scope) {
		NumberProxy* proxy = new NumberProxy(address, type, scope);
		proxy->numberType = NumberType::INT_T;
		return proxy;
	}

	ObjectProxy* NumberProxy::doubleConstruct(const TDataMember& type, TClassRef scope) {
		NumberProxy* proxy = new NumberProxy(type, scope);
		proxy->numberType = NumberType::DOUBLE_T;
		return proxy;
	}

	ObjectProxy* NumberProxy::doubleConstruct(void *address, const TGlobal& type, TClassRef scope) {
		NumberProxy* proxy = new NumberProxy(address, type, scope);
		proxy->numberType = NumberType::DOUBLE_T;
		return proxy;
	}
}
