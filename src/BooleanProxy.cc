#include "BooleanProxy.h"

namespace rootJS
{
	bool BooleanProxy::isBoolean(std::string type)
	{
		if (std::regex_match(type, std::regex("bool"))) {
			return true;
		}
		else if (std::regex_match(type, std::regex("Bool_t"))) {
			return true;
		}

		// TODO
		return false;
	}

	BooleanProxy::BooleanProxy(const TDataMember& type, TClassRef scope)
		: PrimitiveProxy(type, scope)
	{
		// TODO
	}
}
