#include "PrimitiveProxy.h"

namespace rootJS {

	PrimitiveProxy::PrimitiveProxy(ProxyMode& type, TClassRef scope): ObjectProxy(type, scope) {

	}

	bool PrimitiveProxy::isPrimitive() {
		return true;
	}
}
