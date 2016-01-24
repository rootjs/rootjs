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
			return v8::Number::New(v8::Isolate::GetCurrent(), castToDouble(getAddress()));
		}
		return getProxy();
	}

	Double_t NumberProxy::castToDouble(void *ptr) {
		switch(numberType) {
		case NumberType::INT_T:
			return *((Int_t*)getAddress());
		case NumberType::DOUBLE_T:
			return *((Double_t*)getAddress());
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

	void NumberProxy::setValue(v8::Local<v8::Value> value) {
		if(isConst()) {
			v8::Isolate::GetCurrent()->ThrowException(
				v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
				"This value cannot be overwritten, it's constant."
			));
		}
		double numberValue;
		if(value->IsNumberObject()) {
			numberValue = v8::Local<v8::NumberObject>::Cast(value)->NumberValue();
		} else if(value->IsNumber()) {
			numberValue = v8::Local<v8::Number>::Cast(value)->Value();
		} else {
			v8::Isolate::GetCurrent()->ThrowException(
				v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
				"This element can only store numbers."
			));
			return;
		}

		switch(numberType) {
			case NumberType::INT_T:
				*((int*)getAddress()) = (int)numberValue;
				break;
			case NumberType::DOUBLE_T:
				*((double*)getAddress()) = numberValue;
				break;
		}
	}
}
