#include "BooleanProxy.h"
#include "PointerInfo.h"
#include "Toolbox.h"

namespace rootJS
{

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
		return Nan::New(*b);
	}

	void BooleanProxy::setValue(v8::Local<v8::Value> value)
	{
		if(isConst()) {
			Toolbox::throwException("This value cannot be overwritten, it's constant.");
			return;
		}

		bool* b = (bool*)getAddress();
		if(value->IsBoolean() || value->IsBooleanObject()) {
			*b = value->ToBoolean()->Value();
		} else {
			Toolbox::throwException("This element can only store a bool value.");
		}
	}
}
