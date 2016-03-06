#ifndef PRIMITIVE_PROXY_H
#define PRIMITIVE_PROXY_H

#include "ObjectProxy.h"

namespace rootJS
{
	/**
	 * Maps a C++/ROOT primitive to a JavaScript primitive
	 */
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

	};
}

#endif
