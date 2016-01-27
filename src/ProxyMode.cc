#include "ProxyMode.h"

namespace rootJS {

ProxyMode::ProxyMode(const TObject & obj) {
}
ProxyMode::~ProxyMode() {
}
bool ProxyMode::isGlobal() {
	return false;
}
Long_t ProxyMode::GetOffset(){
	return 0;
}
}
