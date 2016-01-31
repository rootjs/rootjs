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

#include "MetaInfo.h"

namespace rootJS
{
	typedef ObjectProxy* (*PrimitiveProxyInitializator)(MetaInfo*, TClass*);

	/*
	typedef ObjectProxy* (*MemberProxyInitializator)(MetaInfo*, TClass*);
	typedef ObjectProxy* (*GlobalProxyInitializator)(MetaInfo*, TClass*);
	 */

	class ObjectProxyFactory
	{
		private:
			ObjectProxyFactory(void);

			static const char* getClassNameFromType(const char*);

			static void traverseClass(TClass *clazz, ObjectProxy &holder);

			static std::map<std::string, PrimitiveProxyInitializator> primitiveProxyMap;

			/*
			static std::map<std::string, MemberProxyInitializator> memberProxyMap;
			static std::map<std::string, GlobalProxyInitializator> globalProxyMap;
			*/

		public:
			static ObjectProxy* createObjectProxy(void *address, MetaInfo *type, TClass *scope);

			static ObjectProxy* createObjectProxy(ObjectProxy &holder, MetaInfo *type, TClass *scope);

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
			static ObjectProxy* createObjectProxy(void* address, MetaInfo *type, TClass *scope, v8::Local<v8::Object> proxy);

			// static ObjectProxy* createObjectProxy(void* address, TClassRef &type, v8::Local<v8::Object> proxy);

			static ObjectProxy* determineProxy(MetaInfo *info, TClass *scope);

			static void initializeProxyMap();
	};
}

#endif
