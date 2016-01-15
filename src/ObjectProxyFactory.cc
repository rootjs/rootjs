#include "ObjectProxyFactory.h"

#include "Proxy.h"

#include <v8.h>
#include <TClassRef.h>
#include <TClassTable.h>
#include <TDataMember.h>
#include <TList.h>

namespace RootJS {
  ObjectProxy* ObjectProxyFactory::createObjectProxy(TObject * object) {
    TClass *klass = gClassTable->GetDict(object->GetName())();
    TList *propertieList = klass->GetListOfAllPublicDataMembers();
    TListIter nextProperty(propertieList);

    TClassRef *classRef = new TClassRef(klass);
    ObjectProxy *proxy = new ObjectProxy(object);

    while(TDataMember *member = (TDataMember*)nextProperty()) {
      v8::Local<v8::Object> nodeObject = proxy->getObject();
      ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(*member, *classRef, *proxy);
      nodeObject->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName()), memberProxy->get());
    }

    delete classRef;

    return proxy;
  }

  ObjectProxy* ObjectProxyFactory::createObjectProxy(const TDataMember & type, TClassRef scope, ObjectProxy & holder) {
    TObject *object = (TObject*)(holder.getAddress() + type.GetOffsetCint());
    object->GetName(); /* Hide unused warning */
    return nullptr;
  }
}
