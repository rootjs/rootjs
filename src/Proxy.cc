#include "Proxy.h"

RootJS::Proxy::Proxy(void* address, const TObject & type, TClassRef & scope) :
	address(address), type(type), scope(scope) {

}

RootJS::Proxy::~Proxy() {
}

void RootJS::Proxy::setAddress(void* address) {
	this->address = address;
}

void* RootJS::Proxy::getAddress() {
	return address;
}

TClassRef& RootJS::Proxy::getScope() {
	return scope;
}

const TObject& RootJS::Proxy::getType() {
	return type;
}
