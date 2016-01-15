#include "Proxy.h"

RootJS::Proxy::Proxy(TObject* address, TObject & type, TClassRef & scope) :
		address(address), type(type), scope(scope) {

}

RootJS::Proxy::~Proxy() {
}

void RootJS::Proxy::setAddress(TObject* address) {
	this->address = address;
}

TObject* RootJS::Proxy::getAddress() {
	return address;
}

TClassRef& RootJS::Proxy::getScope() {
	return scope;
}

TObject& RootJS::Proxy::getType() {
	return type;
}
