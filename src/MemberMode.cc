#include "MemberMode.h"

namespace RootJS {

MemberMode::MemberMode(TDataMember &type) { currentObject = type; }

bool ProxyMode::isGlobal() { return false; }
Long_t ProxyMode::GetAddress() {
  return static_cast<TDataMember>(currentObject).GetOffset();
}
}
