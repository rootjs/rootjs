#include "MemberMode.h"

namespace RootJS {

MemberMode::MemberMode(TDataMember &type) { currentObject = type; }

bool MemberMode::isGlobal() { return false; }
Long_t MemberMode::GetAddress() {
  return static_cast<TDataMember>(currentObject).GetOffset();
}
}
