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

			static ObjectProxy* createObjectProxy(MetaInfo &info, TClass *scope, v8::Local<v8::Object>* instancePtr) throw(std::invalid_argument);
			static std::map<std::string, ObjectProxy*>* createPropertyMap(MetaInfo &info, TClass *clazz, ObjectProxy *holder) throw(std::invalid_argument);

			static TClass* getClass(std::string const& typeName);
			static TEnumConstant* getEnumConstant(std::string const& typeName);
			static bool resolveTypeName(MetaInfo &info, std::string &trueType);

			ObjectProxyFactory();

		public:
            /**
			 *	Encapsulate the supplied data into a new JavaScript object.
			 *
			 *	@param info
			 *			the type info of the data
			 *
			 *	@param scope
			 *			the enclosing scope of the data
			 *
			 *	@return a new ObjectProxy holding a generated JavaScript object for exposure
			 */
			static ObjectProxy* createObjectProxy(MetaInfo &info, TClass *scope) throw(std::invalid_argument);

            /**
			 *	Encapsulate the supplied data into the specified JavaScript object.
			 *
			 *	@param info
			 *			the type info of the data
			 *
			 *	@param scope
			 *			the enclosing scope of the data
			 *
			 *	@param instance
			 *			the JavaScript object to encapsulate the supplied data in
			 *
			 *	@return a new ObjectProxy holding the supplied JavaScript object for exposure
			 */
			static ObjectProxy* createObjectProxy(MetaInfo &info, TClass *scope, v8::Local<v8::Object> instance) throw(std::invalid_argument);

			static ObjectProxy* createPrimitiveProxy(MetaInfo &info, TClass *scope);

			static ObjectProxy* createEnumProxy(MetaInfo &info, TClass *scope);

            /**
             *  Initializes the ProxyMap with standard data types.
             */
			static void initializeProxyMap(void);
	};
}

#endif
