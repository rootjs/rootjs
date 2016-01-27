#include "PrimitiveProxy.h"

namespace rootJS {

	PrimitiveProxy::PrimitiveProxy(const TDataMember& type, TClassRef scope): ObjectProxy(type, scope) {

	}

	PrimitiveProxy::PrimitiveProxy(void *object, const TGlobal & type, TClassRef scope): ObjectProxy(object, type, scope) {

	}

	bool PrimitiveProxy::isPrimitive() {
		return true;
	}
}
