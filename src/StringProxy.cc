#include "StringProxy.h"

#include "Toolbox.h"
#include "PointerInfo.h"

namespace rootJS
{
	StringProxy::StringProxy(MetaInfo &info, TClass *scope) : PrimitiveProxy(info, scope)
	{}

	bool StringProxy::isString(std::string type)
	{
		// TODO Why?!
		return false;
	}

	StringProxy::~StringProxy() {
		if(backedUp) {
			switch(strType) {
				case StringType::CHAR:
					free(*(char**)getAddress());
					break;
				case StringType::STRING:
					delete (*(std::string**)getAddress());
					break;
				case StringType::TSTRING:
					delete (*(TString**)getAddress());
					break;
			}
			free(getAddress());
		}
	}

	v8::Local<v8::Value> StringProxy::get() {
		const char* c = c_str();
		if(c == nullptr)
		{
			return v8::Null(v8::Isolate::GetCurrent());
		}
		else
		{
			return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), c);
		}
	}

	const char* StringProxy::c_str()
	{
		switch(strType)
		{
		case StringType::CHAR:
			return *(char**)getAddress();
		case StringType::STRING:
			{
				std::string *str = (std::string*)getAddress();
				return str->c_str();
			}
		case StringType::TSTRING:
			{
				TString *tstr = (TString*)getAddress();
				return tstr->Data();
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

	void StringProxy::setValue(v8::Local<v8::Value> value)
	{
		if(isConst())
		{
			Toolbox::throwException("This value cannot be overwritten, it's constant.");
		}

		if(!value->IsString() && !value->IsStringObject())
		{
			Toolbox::throwException("This element can only store strings.");
		}

		v8::String::Utf8Value str(value->ToString());
		std::string charValue(*str);

		switch(strType)
		{
		case StringType::CHAR:
			{
				Toolbox::throwException("This value cannot be overwritten, it's a char pointer.");
				break;
			}
		case StringType::STRING:
			{
				std::string *strObj = (std::string*)getAddress();
				*strObj = charValue.c_str();
				break;
			}
		case StringType::TSTRING:
			{
				TString *strObj = (TString*)getAddress();
				*strObj = charValue.c_str();
				break;
			}
		}
	}

	void StringProxy::backup() {
		void **ptrptr = (void**)malloc(sizeof(void*));
		switch(strType) {
			case StringType::CHAR:{
				char* backupPtr = (char*)malloc(strlen(*(char**)getAddress()) + 1);
				strncpy(backupPtr, *(char**)getAddress(), strlen(*(char**)getAddress()));
				backupPtr[strlen(*(char**)getAddress())] = '\0';
				*ptrptr = backupPtr;
				break;
			}
			case StringType::STRING: {
				std::string *backupStr = new std::string(*(std::string*)getAddress());
				*ptrptr = backupStr;
				break;
			}
			case StringType::TSTRING: {
				TString *backupTStr = new TString(*(TString*)getAddress());
				*ptrptr = backupTStr;
				break;
			}
		}

		const char* typeName = info->getTypeName();
		delete info;
		info = new PointerInfo(ptrptr, typeName);
		backedUp = true;
	}
}
