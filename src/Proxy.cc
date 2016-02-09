#include "Proxy.h"

namespace rootJS
{

	Proxy::Proxy(MetaInfo &info, TClass *scope) : info(info.clone()), scope(scope)
	{}

	Proxy::~Proxy()
	{
		delete info;
	}

	void Proxy::setAddress(void* address)
	{
		//this->address = address;
	}

	void* Proxy::getAddress()
	{
		return info->getAddress();
	}

	TClass* Proxy::getScope()
	{
		return scope.GetClass();
	}

	MetaInfo* Proxy::getTypeInfo()
	{
		return info;
	}

}
