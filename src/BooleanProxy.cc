#include "BooleanProxy.h"

namespace rootJS
{
	bool BooleanProxy::isBoolean(std::string type)
	{
		/*
		if (std::regex_match(type, std::regex("bool"))) {
			return true;
		}
		else if (std::regex_match(type, std::regex("Bool_t"))) {
			return true;
		}
		*/

		// TODO
		return false;
	}

	BooleanProxy::BooleanProxy(void* address, MetaInfo *info, TClass *scope) : PrimitiveProxy(address, info, scope)
	{

	}

	BooleanProxy::BooleanProxy(MetaInfo *info, TClass *scope) : BooleanProxy(nullptr, info, scope)
	{

	}


}
