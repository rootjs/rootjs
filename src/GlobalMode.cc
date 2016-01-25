#include "GlobalMode.h"

namespace RootJS {

bool ProxyMode::isGlobal() { return true; }
Long_t ProxyMode::GetAddress() { return 0; }
}
