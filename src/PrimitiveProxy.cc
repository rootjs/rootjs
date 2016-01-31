#include "PrimitiveProxy.h"

namespace rootJS {

	PrimitiveProxy::PrimitiveProxy(void* address, MetaInfo *info, TClass *scope): ObjectProxy(address, info, scope) {

	}

	PrimitiveProxy::PrimitiveProxy(MetaInfo *info, TClass *scope): ObjectProxy(nullptr, info, scope) {

	}

	bool PrimitiveProxy::isPrimitive() {
		return true;
	}
}
