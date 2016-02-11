#ifndef TYPES_H
#define TYPES_H

#include <v8.h>

namespace rootJS
{
	class Types
	{
		public:
			static bool isV8Boolean(v8::Local<v8::Value> value);
			static bool isV8Number(v8::Local<v8::Value> value);
			static bool isV8String(v8::Local<v8::Value> value);

			static bool isV8Primitive(v8::Local<v8::Value> value);
	};
}

#endif // TYPES_H
