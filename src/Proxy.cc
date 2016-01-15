#include "Proxy.h"

RootJS::Proxy::Proxy(void* address, TObject & type, TClassRef & scope) :
		address(address), type(type), scope(scope) {

}

RootJS::Proxy::~Proxy() {
}

void RootJS::Proxy::setAdress(void* address) {
	this->address = address;
}

void* RootJS::Proxy::getAddress() const {
	return address;
}

TClassRef& RootJS::Proxy::getScope() {
	return scope;
}

TObject& RootJS::Proxy::getType() {
	return type;
}
