#include "PointerInfo.h"

namespace rootJS {
	PointerInfo::PointerInfo(void* baseAddr, const char* typeName)
		: MetaInfo(TObject(), baseAddr), typeName(typeName) {
	}
}
