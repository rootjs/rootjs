#ifndef TYPES_H
#define TYPES_H

#include <v8.h>

namespace rootJS
{
	class Types
	{
		public:
            /**
             * Checks if the given value is an v8 Boolean.
             *
             * @param value
             *              The value to checked
             */
			static bool isV8Boolean(v8::Local<v8::Value> value);
            /**
             * Checks if the given value is an v8 Number.
             *
             * @param value
             *              The value to checked
             */
			static bool isV8Number(v8::Local<v8::Value> value);
            /**
             * Checks if the given value is an v8 String.
             *
             * @param value
             *              The value to checked
             */
			static bool isV8String(v8::Local<v8::Value> value);
            /**
             * Checks if the given value is an v8 Primitive.
             *
             * @param value
             *              The value to checked
             */
			static bool isV8Primitive(v8::Local<v8::Value> value);
	};
}

#endif // TYPES_H
