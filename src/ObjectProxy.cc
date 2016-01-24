#include "ObjectProxy.h"

#include <TObject.h>
#include <TGlobal.h>

namespace RootJS {

RootJS::ObjectProxy::ObjectProxy(TDataMember &type, TClassRef scope)
    : Proxy(nullptr, type, scope) {
  this->currentmode = MemberMode(type);
}

RootJS::ObjectProxy::ObjectProxy(TGlobal &type, TClassRef scope)
    : Proxy(nullptr, type, scope) {
  this->currentmode = GlobalMode(type);
}

RootJS::ObjectProxy::~ObjectProxy() {}

const const TDataMember &RootJS::ObjectProxy::getType() {
  return dynamic_cast<const TDataMember &>(type);
}

char *const RootJS::ObjectProxy::getTypeName() { return nullptr; }

void RootJS::ObjectProxy::set(ObjectProxy &value) {
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

bool RootJS::ObjectProxy::isPrimitive() { return false; }

bool RootJS::ObjectProxy::isTemplate() {
  return false; // TODO
}

bool RootJS::ObjectProxy::isGlobal() { return currentmode.isGlobal(); }

bool RootJS::ObjectProxy::isConst() {
  return false; // TODO
}

bool RootJS::ObjectProxy::isStatic() {
  return false; // TODO
}
}
