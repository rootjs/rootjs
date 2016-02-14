#include "ArrayProxy.h"

namespace rootJS
{
	ArrayProxy::ArrayProxy(MetaInfo &info, TClass *scope)
		: ObjectProxy(info, scope)
	{
	}

	ArrayProxy::~ArrayProxy()
	{
		// TODO
	}
}
