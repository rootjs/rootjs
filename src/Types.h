#ifndef TYPES_H
#define TYPES_H

#include <v8.h>
#include <string>
#include <TDataType.h>

namespace rootJS
{
	class Types
	{
		public:
			/**
			 * Gets the normalized (no typedef, ...) TDataType which matches name
			 * @return nullpointer if there is no matching datatype
			 */
			static TDataType* getTypeByName(const std::string &name);

			static bool isV8Boolean(v8::Local<v8::Value> value);
			static bool isV8Number(v8::Local<v8::Value> value);
			static bool isV8String(v8::Local<v8::Value> value);

			static bool isV8Primitive(v8::Local<v8::Value> value);
	};
}

#endif // TYPES_H
