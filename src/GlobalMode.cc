#include "GlobalMode.h"

namespace rootJS {
	GlobalMode::GlobalMode(const  TGlobal& type)
		: ProxyMode(type, type.GetAddress()), currentObject(type) {

	}
	bool GlobalMode::isGlobal() {
		return true;
	}

	GlobalMode::~GlobalMode() {
	}

	Long_t GlobalMode::GetOffset() {
		return 0;
	}

	bool GlobalMode::isConst() {
		if(currentObject.Property() & kIsConstant) {
			return true;
		} else {
			return false;
		}
	}

	const char* GlobalMode::getTypeName() {
		return currentObject.GetTypeName();
	}
}
