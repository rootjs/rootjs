#include "Types.h"

#include <v8.h>

namespace rootJS
{
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
