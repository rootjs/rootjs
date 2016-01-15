#ifndef OBJECT_PROXY_FACTORY_H
#define OBJECT_PROXY_FACTORY_H

#include "ObjectProxy.h"
#include <TObject.h>
#include <TDataMember.h>
#include <TClassRef.h>

namespace RootJS {
  class ObjectProxyFactory {
  private:
    ObjectProxyFactory(void);
  public:
    static ObjectProxy* createObjectProxy(TObject * object);
    static ObjectProxy* createObjectProxy(const TDataMember&, TClassRef, ObjectProxy&);
  };
}

#endif /*OBJECT_PROXY_FACTORY_H*/
