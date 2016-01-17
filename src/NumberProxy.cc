#include "NumberProxy.h"

namespace RootJS {

  NumberProxy::NumberProxy(const TDataMember& type, TClassRef scope): PrimitiveProxy(type, scope){

  }

  NumberProxy::NumberProxy(void * object, const TGlobal & type, TClassRef scope): PrimitiveProxy(object, type, scope){

  }

  v8::Local<v8::Value> NumberProxy::get() {
    if(getAddress()) {
      return v8::NumberObject::New(v8::Isolate::GetCurrent(), *((Int_t*)getAddress()));
    }
    return getProxy();
  }

  ObjectProxy* NumberProxy::construct(const TDataMember& type, TClassRef scope) {
    return new NumberProxy(type, scope);
  }

  ObjectProxy* NumberProxy::construct(void *address, const TGlobal& type, TClassRef scope) {
    return new NumberProxy(address, type, scope);
  }
}
