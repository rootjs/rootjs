#ifndef OBJECT_PROXY_FACTORY_H
#define OBJECT_PROXY_FACTORY_H

#include "ObjectProxy.h"

#include <string>
#include <map>
#include <v8.h>

#include <TGlobal.h>
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

		/**
		 *	Encapsulate the data at the specified address into the specified JavaScript object.
		 *
		 *	@param address
		 *			the address of the data which should be encapsulated
		 *
		 *	@param type
		 *			the type of the data which should be encapsulated
		 *
		 *	@param proxy
		 *			the JavaScript object used for encapsulation
		 *
		 */
		static void createObjectProxy(void* address, TClassRef &type, v8::Local<v8::Object> proxy);
		// static ObjectProxy* createObjectProxy(void* address, TClassRef &type, v8::Local<v8::Object> proxy);

		static ObjectProxy* determineProxy(const TDataMember&, TClassRef);
		static ObjectProxy* determineProxy(void*, const TGlobal&, TClassRef);

		static void initializeProxyMap(void);
	};
}

#endif /*OBJECT_PROXY_FACTORY_H*/
