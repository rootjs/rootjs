#ifndef BOOLEAN_PROXY_H
#define BOOLEAN_PROXY_H

#include "PrimitiveProxy.h"

#include <TClassRef.h>
#include <TDataMember.h>

namespace RootJS
{
	class BooleanProxy: public PrimitiveProxy
	{
	public:
		/**
		 * Create a new BooleanProxy.
		 *
		 * @param type the type of the excapsulated object
		 * @param scope the scope of the encapsulated object
		 */
		BooleanProxy(TDataMember type, TClassRef scope);
	};
}

#endif // BOOLEAN_PROXY_H
