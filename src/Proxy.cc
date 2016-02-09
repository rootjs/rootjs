#include "Proxy.h"

namespace rootJS
{

	Proxy::Proxy(rootJS::MetaInfo& type, TClassRef & scope) : type(type.clone()), scope(TClassRef(scope))
	{}

	Proxy::~Proxy()
	{
		delete type;
	}

	void Proxy::setAddress(void* address)
	{
		//this->address = address;
	}

	void* Proxy::getAddress()
	{
		return type->getAddress();
	}

	TClassRef& Proxy::getScope()
	{
		return scope;
	}

	MetaInfo* Proxy::getType()
	{
		return type;
	}

}
