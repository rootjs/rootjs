#include "StringProxy.h"

#include "Toolbox.h"
#include "PointerInfo.h"

namespace rootJS
{
	StringProxy::StringProxy(MetaInfo &info, TClass *scope) : PrimitiveProxy(info, scope)
	{}


	StringProxy::~StringProxy()
	{

	}

	v8::Local<v8::Value> StringProxy::get()
	{
		const char* c = c_str();
		if(c == nullptr) {
			return Nan::Null();
		} else {
			return Nan::New(c).ToLocalChecked();
		}
	}

	const char* StringProxy::c_str()
	{
		switch(strType) {
		case StringType::CHAR:
			return (char*)getAddress();
		case StringType::STRING: {
			std::string *str = (std::string*)getAddress();
			return str->c_str();
		}
		case StringType::TSTRING: {
			TString *tstr = (TString*)getAddress();
			return tstr->Data();
		}
		case StringType::SINGLE_CHAR: {
			singleChar[0] = **(char**)getAddress();
			singleChar[1] = '\0';
			return singleChar;
		}
		}
		return "Failed to load c_str from this var";
	}

	ObjectProxy* StringProxy::charConstruct(MetaInfo &info, TClass *scope)
	{
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::CHAR;
		return proxy;
	}

	ObjectProxy* StringProxy::stringConstruct(MetaInfo &info, TClass *scope)
	{
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::STRING;
		return proxy;
	}

	ObjectProxy* StringProxy::tStringConstruct(MetaInfo &info, TClass *scope)
	{
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::TSTRING;
		return proxy;
	}

	ObjectProxy* StringProxy::singleCharConstruct(MetaInfo &info, TClass *scope)
	{
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::SINGLE_CHAR;
		return proxy;
	}

	void StringProxy::setValue(v8::Local<v8::Value> value)
	{
		if(isConst()) {
			Toolbox::throwException("This value cannot be overwritten, it's constant.");
			return;
		}

		if(!value->IsString() && !value->IsStringObject()) {
			Toolbox::throwException("This element can only store strings.");
			return;
		}

		v8::String::Utf8Value str(value->ToString());
		std::string charValue(*str);

		switch(strType) {
		case StringType::CHAR: {
			Toolbox::throwException("This value cannot be overwritten, it's a char pointer.");
			break;
		}
		case StringType::SINGLE_CHAR: {
			Toolbox::throwException("This value cannot be overwritten, it's a char pointer.");
			break;
		}
		case StringType::STRING: {
			std::string *strObj = (std::string*)getAddress();
			*strObj = charValue.c_str();
			break;
		}
		case StringType::TSTRING: {
			TString *strObj = (TString*)getAddress();
			*strObj = charValue.c_str();
			break;
		}
		}
	}
}
