#ifndef STRING_PROXY_H
#define STRING_PROXY_H

#include "PrimitiveProxy.h"

#include <string>

#include <TClassRef.h>
#include <TDataMember.h>

namespace rootJS
{
	/**
	 * Proxies a string.
	 */
	class StringProxy: public PrimitiveProxy
	{
		public:
			/**
			 * Check if the type is a boolean type.
			 *
			 * @param type the type to be checked
			 * @return if the type is a string type.
			 */
			static bool isString(std::string type);

			/**
			 * Create a new StringProxy.
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			StringProxy(void *address, MetaInfo *info, TClass *scope);

			StringProxy(MetaInfo *info, TClass *scope);
	};
}

#endif // STRING_PROXY_H
