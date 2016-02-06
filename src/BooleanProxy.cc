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

	BooleanProxy::BooleanProxy(MetaInfo& type, TClassRef scope)
		: PrimitiveProxy(type, scope)
	{
		// TODO
	}

	ObjectProxy* BooleanProxy::boolConstruct(MetaInfo& type, TClassRef scope){
		return new BooleanProxy(type, scope);
	}

	v8::Local<v8::Value> BooleanProxy::get() {
		bool* b = (bool*)getAddress();
		return v8::Boolean::New(v8::Isolate::GetCurrent(), *b);
	}
}
