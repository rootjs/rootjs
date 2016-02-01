#include "GlobalInfo.h"

namespace rootJS {
	GlobalInfo::GlobalInfo(const  TGlobal& type)
		: MetaInfo(type, nullptr), currentObject(type) {
		baseAddress = type.GetAddress();
	}
	bool GlobalInfo::isGlobal() {
		return true;
	}

	GlobalInfo::~GlobalInfo() {
	}

	Long_t GlobalInfo::GetOffset() {
		return 0;
	}

	bool GlobalInfo::isConst() {
		if(currentObject.Property() & kIsConstant) {
			return true;
		} else {
			return false;
		}
	}

	const char* GlobalInfo::getTypeName() {
		return currentObject.GetTypeName();
	}
}
