#include "GlobalMode.h"

namespace RootJS {
GlobalMode::GlobalMode(const  TGlobal& type) : ProxyMode(type), currentObject(type){

}
bool ProxyMode::isGlobal() { return true; }
Long_t getOffset() { return 0; }
}

RootJS::ProxyMode::ProxyMode(const TObject& foo) {
}

RootJS::ProxyMode::~ProxyMode() {
}

Long_t RootJS::ProxyMode::GetOffset() {
	return 0;
}
