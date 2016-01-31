#include "Proxy.h"

namespace rootJS {

	const TClass* Proxy::GLOBAL_SCOPE = (TClass*) -1;

	Proxy::Proxy(void* address, TClass *scope) : address(address), scopeRef(scope) {

	}

	Proxy::~Proxy() {
	}

	void Proxy::setAddress(void* address) {
		this->address = address;
	}

	void* Proxy::getAddress() {
		return address;
	}

	TClass* Proxy::getScope() {
		return scopeRef.GetClass();
	}

}
