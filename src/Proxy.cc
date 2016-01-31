#include "Proxy.h"

rootJS::Proxy::Proxy(rootJS::ProxyMode& type, TClassRef & scope) :
	type(type.clone()), scope(TClassRef(scope)) {

}

rootJS::Proxy::~Proxy() {
	delete type;
}

void rootJS::Proxy::setAddress(void* address) {
	//this->address = address;
}

void* rootJS::Proxy::getAddress() {
	return type->getAddress();
}

TClassRef& rootJS::Proxy::getScope() {
	return scope;
}

rootJS::ProxyMode* rootJS::Proxy::getType() {
	return type;
}
