#include "MemberMode.h"

RootJS::MemberMode::MemberMode(TDataMember &type) { currentObject = type; }

bool RootJS::ProxyMode::isGlobal() { return false; }
