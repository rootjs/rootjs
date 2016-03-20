#include "EnumConstProxy.h"

#include "Types.h"
#include "Toolbox.h"

#include <RtypesCore.h>
#include <TEnum.h>
#include <TCollection.h>

namespace rootJS
{

	EnumConstProxy::EnumConstProxy(MetaInfo &info, TClass *scope) : PrimitiveProxy(info, scope)
	{}

	EnumConstProxy::~EnumConstProxy()
	{}

	v8::Local<v8::Value> EnumConstProxy::get()
	{
		return v8::Number::New(v8::Isolate::GetCurrent(), *((Long64_t*) (info->getAddress())));
	}

	void EnumConstProxy::setValue(v8::Local<v8::Value> value)
	{
		if(isConst())
		{
			Toolbox::throwException("Variable is constant. Can not assign the specified value.");
			return;
		}

		if(value->IsNull())
		{
			Toolbox::throwException("Specified value is null. Can not assign the specified value.");
			return;
		}

		std::string typeName(getTypeName());
		Types::resolveTypeName(*info, typeName);

		TEnumConstant *eConst = fromValue(value, typeName);
		if(eConst != nullptr && eConst->IsValid())
		{
			*((Long64_t*) getAddress()) = eConst->GetValue(); // value found => assign
		}
		else
		{
			Toolbox::throwException("Invalid type.");
			return;
		}
	}

	TEnumConstant* EnumConstProxy::fromValue(v8::Local<v8::Value> value, std::string typeName)
	{
		if(value->IsNull())
		{
			return nullptr;
		}

		if(value->IsNumber() || value->IsNumberObject())
		{
			// check if the number appears in the corporate enum
			return fromNumber(value->ToNumber(), typeName);
		}
		else if(value->IsObject())
		{
			v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);

			// check if the proxy object encapsulates an ObjectProxy
			if(object->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
			{
				return nullptr;
			}

			// Obtain the encapsulated proxy from the internal field
			ObjectProxy* proxy = (ObjectProxy*)(object->GetAlignedPointerFromInternalField(Toolbox::ObjectProxyPtr));
			if(proxy == nullptr)
			{
				return nullptr;
			}

			// check if proxy encapsulates a number primitive
			if(proxy->get()->IsNumberObject() || proxy->get()->IsNumber())
			{
				return fromNumber(proxy->get()->ToNumber(), typeName);
			}
		}

		return nullptr;
	}

	TEnumConstant* EnumConstProxy::fromNumber(v8::Local<v8::Number> number, std::string typeName)
	{
		if(number->IsNull())
		{
			return nullptr;
		}

		Long64_t value = (Long64_t) number->IntegerValue();

		// Find enum
		TEnum *e = TEnum::GetEnum(typeName.c_str());
		if(e == nullptr)	// should never happen
		{
			return nullptr;
		}

		// check if value of an enum constant equals value of supplied number
		TIter enumIter(e->GetConstants());
		TEnumConstant *eConst = nullptr;
		while ( (eConst = (TEnumConstant*) enumIter()))
		{
			if(eConst->IsValid() && (eConst->GetValue() == value))
			{
				return eConst;
			}
		}

		return nullptr;
	}
}
