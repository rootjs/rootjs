#include "PrimitiveProxy.h"

namespace rootJS {

	PrimitiveProxy::PrimitiveProxy(MetaInfo& type, TClassRef scope): ObjectProxy(type, scope) {

	}

	bool PrimitiveProxy::isPrimitive() {
		return true;
	}
}
