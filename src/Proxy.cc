#include "Proxy.h"

rootJS::Proxy::Proxy(void* address, const TObject & type, TClassRef & scope) :
	address(address), type(type), scope(scope) {

}

rootJS::Proxy::~Proxy() {
}

void rootJS::Proxy::setAddress(void* address) {
	this->address = address;
}

void* rootJS::Proxy::getAddress() {
	return address;
}

TClassRef& rootJS::Proxy::getScope() {
	return scope;
}

const TObject& rootJS::Proxy::getType() {
	return type;
}
