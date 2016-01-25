#include "MemberMode.h"
namespace RootJS {

MemberMode::MemberMode(const TDataMember& type) : ProxyMode(type)  {
currentObject = &type;
}

bool MemberMode::isGlobal() {
	return false;
}
Long_t MemberMode::GetOffset() {


		return currentObject->GetOffset();
}
}
