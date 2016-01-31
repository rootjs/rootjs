#ifndef BOOLEAN_PROXY_H
#define BOOLEAN_PROXY_H

#include "PrimitiveProxy.h"

#include <string>
#include <regex>

#include <TClassRef.h>
#include <TDataMember.h>

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
			 */
			BooleanProxy(MetaInfo *info, TClass *scope);

			/**
			 * Create a new BooleanProxy.
			 *
			 */
			BooleanProxy(void *address, MetaInfo *info, TClass *scope);
	};
}

#endif
