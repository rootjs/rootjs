#ifndef PRIMITIVE_PROXY_H
#define PRIMITIVE_PROXY_H

#include "ObjectProxy.h"

namespace rootJS
{
	class PrimitiveProxy: public ObjectProxy
	{
		public:
			/**
			 * Create a new PrimitiveProxy.
			 *
			 * @param info
			 * 			the type of the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			PrimitiveProxy(MetaInfo &type, TClass *scope);

			/**
			 * Check if this proxy encapsulates a primitive type.
			 *
			 * @return true if this ProxyObject encapsulates a primitive data type
			 */
			virtual bool isPrimitive();
	};
}

#endif
