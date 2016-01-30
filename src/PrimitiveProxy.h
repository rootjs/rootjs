#ifndef PRIMITIVE_PROXY_H
#define PRIMITIVE_PROXY_H

#include "ObjectProxy.h"

#include <TDataMember.h>
#include <TClassRef.h>

namespace rootJS {
	class PrimitiveProxy: public ObjectProxy {
		public:
			/**
			 * Create a new PrimitiveProxy.
			 *
			 * @param type
			 * 			the type of the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			PrimitiveProxy(const TDataMember& type, TClassRef scope);

			/**
			 * Creates a new ObjectProxy.
			 * Use this constructor when you do not have a TDataMember
			 * (This happens when you want to proxy an object which is globally available)
			 *
			 * @param object
			 *      A pointer to the object that should be prxied.
			 *      We need a pointer to get the correct address.
			 */
			PrimitiveProxy(const TGlobal & type, TClassRef scope);

			/**
			 * Check if this proxy encapsulates a primitive type.
			 *
			 * @return true if this ProxyObject encapsulates a primitive data type
			 */
			virtual bool isPrimitive();
	};
}

#endif /* PRIMITIVE_PROXY_H */
