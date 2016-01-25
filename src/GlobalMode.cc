#include "GlobalMode.h"

namespace RootJS {
GlobalMode::GlobalMode(const  TGlobal& type) : ProxyMode(type), currentObject(type){

}
bool ProxyMode::isGlobal() { return true; }
Long_t getOffset() { return 0; }
}
