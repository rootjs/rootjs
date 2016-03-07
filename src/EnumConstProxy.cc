#include "EnumConstProxy.h"

#include "Types.h"
#include "Toolbox.h"

#include <RtypesCore.h>
#include <TEnum.h>
#include <TEnumConstant.h>
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

		if(value->IsNumber() || value->IsNumberObject())
		{
			// check if the number appears in the corporate enum
			assignNumber(value->ToNumber());
		}
		else if(value->IsObject())
		{
			v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);

			// check if the proxy object encapsulates an ObjectProxy
			if(object->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
			{
				Toolbox::throwException("Unexpected internal field count.");
				return;
			}

			// Obtain the encapsulated proxy from the internal field
			ObjectProxy* proxy = (ObjectProxy*)(object->GetAlignedPointerFromInternalField(Toolbox::ObjectProxyPtr));
			if(proxy == nullptr)
			{
				Toolbox::throwException("Supplied object does not hold an ObjectProxy.");
				return;
			}

			// check if proxy encapsulates a number primitive
			if(proxy->get()->IsNumberObject() || proxy->get()->IsNumber())
			{
				assignNumber(proxy->get()->ToNumber());
			}
			else
			{
				/*
				// check if both proxies belong to a corporate enum
				std::string typeA(getTypeName());
				std::string typeB(proxy->getTypeName());

				Types::resolveTypeName(*info, typeA);
				Types::resolveTypeName(*(proxy->getTypeInfo()), typeB);

				if(typeA.compare(typeB) == 0)
				{
					Long64_t v = *((Long64_t*) (proxy->getAddress()));
					*((Long64_t*) getAddress()) = v;	// write new value to memory
				}
				else
				{
					Toolbox::throwException("Invalid types. Can not assign value of type '" + typeB + "' to a '" + typeA + "'.");
					return;
				}
				*/
				Toolbox::throwException("Unexpected type.");
				return;
			}
		}
		else
		{
			Toolbox::throwException("Unexpected type.");
			return;
		}
	}

	void EnumConstProxy::assignNumber(v8::Local<v8::Number> number)
	{
		Long64_t value = (Long64_t) number->IntegerValue();

		std::string typeName(getTypeName());
		Types::resolveTypeName(*info, typeName);

		// Find enum
		TEnum *e = TEnum::GetEnum(typeName.c_str());
		if(e == nullptr)	// should never happen
		{
			Toolbox::throwException("Unexpected type.");
			return;
		}

		// check if value of an enum constant equals value of supplied number
		TIter enumIter(e->GetConstants());
		TEnumConstant *eConst = nullptr;
		while ( (eConst = (TEnumConstant*) enumIter()))
		{
			if(eConst->IsValid() && (eConst->GetValue() == value))
			{
				*((Long64_t*) getAddress()) = value; // value found => assign
				return;
			}
		}

		// value not found
		// Toolbox::throwException("Supplied number value '" + Toolbox::Stringv8toStd(number) + "' is not a constant of enum '" + std::string(e->GetQualifiedName()) + "'.");
		Toolbox::throwException("Supplied number value is not a constant of enum '" + std::string(e->GetQualifiedName()) + "'.");
	}

}
