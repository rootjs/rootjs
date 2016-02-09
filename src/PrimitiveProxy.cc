#include "PrimitiveProxy.h"

namespace rootJS {

	PrimitiveProxy::PrimitiveProxy(MetaInfo &type, TClass *scope): ObjectProxy(type, scope) {

	}

	bool PrimitiveProxy::isPrimitive() {
		return true;
	}
}
