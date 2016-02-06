#ifndef BOOLEAN_PROXY_H
#define BOOLEAN_PROXY_H

#include "PrimitiveProxy.h"
#include <string>
#include <regex>

#include <TClassRef.h>
#include <TDataMember.h>
#include "MetaInfo.h"

namespace rootJS
{
	/**
	 * Proxies a bool.
	 */
	class BooleanProxy: public PrimitiveProxy
	{
		public:
			/**
			 * Check if the type is a boolean type.
			 *
			 * @param type the type to be checked
			 * @return if the type is a boolean type
			 */
			static bool isBoolean(std::string type);

			/**
			 * Create a new BooleanProxy.
			 *
			 * @param type the type of the excapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			BooleanProxy(MetaInfo& type, TClassRef scope);

			/**
			 * Creates a BooleanProxy, can be derefered to be added to a map
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy *boolConstruct(MetaInfo& info, TClassRef scope);

			/**
			 * Returns a v8 Boolean depending on MetaInfo
			 *
			 * @return boolean depending on MetaInfo
			 */
			virtual v8::Local<v8::Value> get();
	};
}

#endif // BOOLEAN_PROXY_H
