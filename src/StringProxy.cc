#include "StringProxy.h"

namespace rootJS
{
	bool StringProxy::isString(std::string type)
	{
		// TODO
		return false;
	}

	StringProxy::StringProxy(void *address, MetaInfo *info, TClass *scope) : PrimitiveProxy(address, info, scope)
	{
		// TODO
	}

	StringProxy::StringProxy(MetaInfo *info, TClass *scope) : StringProxy(nullptr, info, scope)
	{
		// TODO
	}

}
