#include "MemberMode.h"
namespace rootJS {

	MemberMode::MemberMode(const TDataMember& type) : ProxyMode(type), currentObject(type)  {

	}
	MemberMode::~MemberMode() {
	}
	bool MemberMode::isGlobal() {
		return false;
	}
	Long_t MemberMode::GetOffset() {
		return currentObject.GetOffset();
	}

	bool MemberMode::isConst() {
		if(currentObject.Property() & kIsConstant) {
			return true;
		} else {
			return false;
		}
	}
	bool MemberMode::isStatic() {
		if(currentObject.Property() & kIsStatic) {
			return true;
		} else {
			return false;
		}
	};
	const char* MemberMode::getTypeName() {
		return currentObject.GetTypeName();
	}
}
