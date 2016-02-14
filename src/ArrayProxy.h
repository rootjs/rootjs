#ifndef ARRAY_PROXY_H
#define ARRAY_PROXY_H

#include "MetaInfo.h"
#include "ObjectProxy.h"

#include <TClass.h>

namespace rootJS
{
	class ArrayProxy : public ObjectProxy
	{
		public:
			/**
			 * Create a new ArrayProxy.
			 *
			 * @param info type info about the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			ArrayProxy(MetaInfo &info, TClass *scope);

			/**
			 * Destroy the ArrayProxy
			 */
			~ArrayProxy();
	};
}

#endif // ARRAY_PROXY_H
