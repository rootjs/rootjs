#ifndef OBJECT_PROXY_FACTORY_H
#define OBJECT_PROXY_FACTORY_H

#include "ObjectProxy.h"
#include "MetaInfo.h"

#include <string>
#include <stdexcept>
#include <map>

#include <v8.h>

#include <TGlobal.h>
#include <TDataMember.h>
#include <TEnumConstant.h>
#include <TClass.h>

namespace rootJS
{
	typedef ObjectProxy* (*ProxyInitializator)(MetaInfo&, TClass*);

	class ObjectProxyFactory
	{
		private:
			static std::map<std::string, ProxyInitializator> primitiveProxyMap;

			static std::map<std::string, ObjectProxy*>* createPropertyMap(MetaInfo &info, TClass *clazz) throw(std::invalid_argument);
			static TClass* getClass(std::string const& typeName);
			static TEnumConstant* getEnumConstant(std::string const& typeName);

			ObjectProxyFactory();

		public:
            /**
			 *	Encapsulate the data at the specified address into the specified JavaScript object.
			 *
			 *	@param global
			 *			the TGlobal which should be encapsulated
			 *
			 *	@return a new ObjectProxy holding the specified JavaScript Object for exposure
			 */
			static ObjectProxy* createObjectProxy(TGlobal &global);

            /**
			 *	Encapsulate the data at the specified address into the specified JavaScript object.
			 *
			 *	@param info
			 *			the info of the Object to be created
			 *
			 *	@param scope
			 *			the type of the data which should be encapsulated
			 *
			 *	@return a new ObjectProxy holding the specified JavaScript Object for exposure
			 */
			static ObjectProxy* createObjectProxy(MetaInfo &info, TClass *scope) throw(std::invalid_argument);

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
			static ObjectProxy* createObjectProxy(void *address, TClass *type, v8::Local<v8::Object> proxy);

            /**
			 *	Encapsulate the data at the specified address into the specified primitive JavaScript object.
			 *
			 *	@param info
			 *			the info of the Object to be created
			 *
			 *	@param clazz
			 *			the type of the data which should be encapsulated
			 *
			 *	@return a new primitive ObjectProxy holding the specified JavaScript Object for exposure
			 */
			static ObjectProxy* createPrimitiveProxy(MetaInfo &info, TClass *clazz);

            /**
             *  Initializes the ProxyMap with standard data types.
             */
			static void initializeProxyMap(void);
	};
}

#endif
