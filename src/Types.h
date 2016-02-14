#ifndef TYPES_H
#define TYPES_H

#include <map>

#include <v8.h>
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

			static const std::map<std::string, std::string> operatorNames;
	};
}

#endif // TYPES_H
