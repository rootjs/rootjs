#include "PointerMode.h"

namespace rootJS {
	PointerMode::PointerMode(void* baseAddr, const char* typeName)
		: ProxyMode(TObject(), baseAddr), typeName(typeName) {
	}
}
