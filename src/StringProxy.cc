#include "StringProxy.h"

namespace rootJS
{
	bool StringProxy::isString(std::string type)
	{
		// TODO Why?!
		return false;
	}

	StringProxy::StringProxy(MetaInfo& info, TClassRef scope)
		: PrimitiveProxy(info, scope)
	{

	}

	v8::Local<v8::Value> StringProxy::get() {
		const char* c = c_str();
		if(c == nullptr) {
			return v8::Null(v8::Isolate::GetCurrent());
		} else {
			return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), c);
		}
	}

	const char* StringProxy::c_str() {
		switch(strType) {
			case StringType::CHAR:
			return *(char**)getAddress();
			case StringType::STRING: {
			std::string *str = (std::string*)getAddress();
			return str->c_str();
		}
			case StringType::TSTRING: {
			TString *tstr = (TString*)getAddress();
			return tstr->Data();
		}
		}
		return "Failed to load c_str from this var";
	}

	ObjectProxy* StringProxy::charConstruct(MetaInfo& info, TClassRef scope) {
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::CHAR;
		return proxy;
	}

	ObjectProxy* StringProxy::stringConstruct(MetaInfo& info, TClassRef scope) {
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::STRING;
		return proxy;
	}

	ObjectProxy* StringProxy::tStringConstruct(MetaInfo& info, TClassRef scope) {
		StringProxy *proxy = new StringProxy(info, scope);
		proxy->strType = StringType::TSTRING;
		return proxy;
	}
}
