#include "Types.h"

#include <v8.h>
#include <TROOT.h>

namespace rootJS
{

	TDataType* Types::getTypeByName(const std::string &stdTypeName)
	{
		std::string cpyTypeName = stdTypeName;
		// check if typeName starts with const
		std::size_t idx = cpyTypeName.find("const ");
		if(cpyTypeName.find('*') != std::string::npos) {
			cpyTypeName = cpyTypeName.substr(0, cpyTypeName.find('*'));
		}
		if(idx != std::string::npos && idx == 0) {
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


	const std::map<std::string, std::string> Types::operatorNames = {
		{"operator==", "_equals"},
		{"operator+=", "_setAdd"},
		{"operator-=", "_setSubstract"},
		{"operator*=", "_setMultiply"},
		{"operator/=", "_setDivide"},
		{"operator++", "_increment"},
		{"operator--", "_decrement"},

		{"operator+", "_add"},
		{"operator-", "_substract"},
		{"operator*", "_multiply"},
		{"operator/", "_divide"},

		{"operator<<=", "_setLeftshift"},
		{"operator>>=", "_setRightshift"},

		{"operator<<", "_leftshift"},
		{"operator>>", "_rightshift"},

		{"operator=", "_set"},
		{"operator new", "_construct"},
		{"operator delete", "_destruct"},

		{"operator()", "_call"},
		{"operator->", "_derefer"},

		{"operator[]", "_at"},
		{"operator delete[]", "_deleteArray"},
		{"operator new[]", "_newArray"},

		{"operator&=", "_setAnd"},
		{"operator!=", "_setNot"},
		{"operator|=", "_setOr"},
		{"operator^=", "_setXOR"},

		{"operator&", "_and"},
		{"operator!", "_not"},
		{"operator|", "_or"},
		{"operator^", "_XOR"},
		{"operator~", "_bitNot"},

		{"delete[]", "_freeArray"},
		{"delete[]", "_freeArray"},
		{"delete[]", "_freeArray"},
		{"delete[]", "_freeArray"},
		{"delete[]", "_freeArray"},
	};
}
