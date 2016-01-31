#ifndef OBJECT_PROXY_FACTORY_H
#define OBJECT_PROXY_FACTORY_H

#include "ObjectProxy.h"
#include "ProxyMode.h"

#include <string>
#include <map>
#include <v8.h>

#include <TGlobal.h>
#include <TDataMember.h>
#include <TClassRef.h>
#include <TClass.h>
#include <TMap.h>

namespace rootJS {
	typedef ObjectProxy* (*ProxyInitializator)(ProxyMode&, TClassRef);

	class ObjectProxyFactory {
		private:
			ObjectProxyFactory(void);
			static const char* getClassNameFromType(const char*);
			static void traverseClass(TClassRef&, ObjectProxy&);
			static std::map<std::string, ProxyInitializator> proxyMap;

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
			 *	@return a new ObjectProxy holding the specified JavaScript Object for exposure
			 */
			static ObjectProxy* createObjectProxy(void* address, TClassRef &type, v8::Local<v8::Object> proxy);

			/**
			 *	Encapsulate the data at the specified address.
			 *
			 *	@param address
			 *			the address of the data which should be encapsulated
			 *
			 *	@param type
			 *			the type of the data which should be encapsulated
			 *
			 *	@return a new ObjectProxy holding a JavaScript Object for exposure
			 */
			static ObjectProxy* createObjectProxy(void* address, TClassRef &type);

			static ObjectProxy* determineProxy(ProxyMode&, TClassRef);

			static void initializeProxyMap(void);
	};
}

#endif /*OBJECT_PROXY_FACTORY_H*/
