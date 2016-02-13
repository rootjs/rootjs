#include "BooleanProxy.h"
#include "PointerInfo.h"
#include "Toolbox.h"

namespace rootJS
{
	bool BooleanProxy::isBoolean(std::string type)
	{
		if (std::regex_match(type, std::regex("bool")))
		{
			return true;
		}
		else if (std::regex_match(type, std::regex("Bool_t")))
		{
			return true;
		}

		// TODO
		return false;
	}

	BooleanProxy::BooleanProxy(MetaInfo &info, TClass *scope) : PrimitiveProxy(info, scope)
	{
		// TODO
	}

	BooleanProxy::~BooleanProxy()
	{
	}

	ObjectProxy* BooleanProxy::boolConstruct(MetaInfo &info, TClass *scope)
	{
		return new BooleanProxy(info, scope);
	}

	v8::Local<v8::Value> BooleanProxy::get()
	{
		bool* b = (bool*)getAddress();
		return v8::Boolean::New(v8::Isolate::GetCurrent(), *b);
	}

	void BooleanProxy::setValue(v8::Local<v8::Value> value)
	{
		if(isConst())
		{
			Toolbox::throwException("This value cannot be overwritten, it's constant.");
			return;
		}

		bool* b = (bool*)getAddress();
		if(value->IsBoolean())
		{
			*b = v8::Local<v8::Boolean>::Cast(value)->Value();
		}
		else if(value->IsBooleanObject())
		{
			*b = v8::Local<v8::BooleanObject>::Cast(value)->ValueOf();
		}
		else
		{
			Toolbox::throwException("This element can only store a bool value.");
		}
	}
}
