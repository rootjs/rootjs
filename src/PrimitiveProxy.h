#ifndef PRIMITIVE_PROXY_H
#define PRIMITIVE_PROXY_H

#include "ObjectProxy.h"

namespace rootJS {
	class PrimitiveProxy: public ObjectProxy {
		public:

			/**
			 * Create a new PrimitiveProxy.
			 *
			 * @param address
			 *
			 * @param info
			 *
			 * @param scope
			 */
			PrimitiveProxy(void* address, MetaInfo *info, TClass *scope);

			/**
			 * Create a new PrimitiveProxy.
			 *
			 * @param info
			 *
			 * @param scope
			 */
			PrimitiveProxy(MetaInfo *info, TClass *scope);

			/**
			 * Check if this proxy encapsulates a primitive type.
			 *
			 * @return true if this ProxyObject encapsulates a primitive data type
			 */
			virtual bool isPrimitive();
	};
}

#endif
