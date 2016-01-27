#include "GlobalMode.h"

namespace rootJS {
GlobalMode::GlobalMode(const  TGlobal& type) : ProxyMode(type), currentObject(type){

}
bool GlobalMode::isGlobal() { return true; }

GlobalMode::~GlobalMode() {
}

Long_t GlobalMode::GetOffset() {
	return 0;
}
}
