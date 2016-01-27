#include "PrimitiveProxy.h"

namespace rootJS {

	PrimitiveProxy::PrimitiveProxy(const TDataMember& type, TClassRef scope): ObjectProxy(type, scope) {

	}

	PrimitiveProxy::PrimitiveProxy(const TGlobal & type, TClassRef scope): ObjectProxy(type, scope) {

	}

	bool PrimitiveProxy::isPrimitive() {
		return true;
	}
}
