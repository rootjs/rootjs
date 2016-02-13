#include "Types.h"

#include <v8.h>
#include <TROOT.h>

namespace rootJS
{

	TDataType* Types::getTypeByName(const std::string &stdTypeName) {
		std::string cpyTypeName = stdTypeName;
		// check if typeName starts with const
		std::size_t idx = cpyTypeName.find("const ");
		if(cpyTypeName.find('*') != std::string::npos) {
			cpyTypeName = cpyTypeName.substr(0, cpyTypeName.find('*'));
		}
		if(idx != std::string::npos && idx == 0)
		{
			return (TDataType*) (gROOT->GetListOfTypes(kTRUE)->FindObject(stdTypeName.substr(6).c_str()));
		}

		return (TDataType*) (gROOT->GetListOfTypes(kTRUE)->FindObject(stdTypeName.c_str()));
	}

	bool Types::isV8Boolean(v8::Local<v8::Value> value)
	{
		return value->IsBoolean() || value->IsBooleanObject();
	}

	bool Types::isV8Number(v8::Local<v8::Value> value)
	{
		return value->IsNumber() || value->IsNumberObject();
	}

	bool Types::isV8String(v8::Local<v8::Value> value)
	{
		return value->IsString() || value->IsStringObject();
	}

	bool Types::isV8Primitive(v8::Local<v8::Value> value)
	{
		return isV8Boolean(value) || isV8Number(value) || isV8String(value);
	}
}
