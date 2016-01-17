#ifndef OBJECT_PROXY_FACTORY_H
#define OBJECT_PROXY_FACTORY_H

#include "ObjectProxy.h"
#include <string>
#include <TGlobal.h>
#include <map>
#include <TDataMember.h>
#include <TClassRef.h>
#include <TClass.h>
#include <TMap.h>

namespace RootJS {
  typedef ObjectProxy* (*MemberProxyInitializator)(const TDataMember&, TClassRef);
  typedef ObjectProxy* (*GlobalProxyInitializator)(void*, const TGlobal&, TClassRef);

  class ObjectProxyFactory {
  private:
    ObjectProxyFactory(void);
    static const char* getClassNameFromType(const char*);
    static void traverseClass(TClassRef&, ObjectProxy&);
    static std::map<std::string, MemberProxyInitializator> memberProxyMap;
    static std::map<std::string, GlobalProxyInitializator> globalProxyMap;
  public:
    static ObjectProxy* createObjectProxy(TGlobal & object);
    static ObjectProxy* createObjectProxy(const TDataMember&, TClassRef, ObjectProxy&);
    static ObjectProxy* determineProxy(const TDataMember&, TClassRef);
    static ObjectProxy* determineProxy(void*, const TGlobal&, TClassRef);

    static void initializeProxyMap(void);
  };
}

#endif /*OBJECT_PROXY_FACTORY_H*/
