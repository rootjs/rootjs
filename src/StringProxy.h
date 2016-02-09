#ifndef STRING_PROXY_H
#define STRING_PROXY_H

#include "PrimitiveProxy.h"

#include <string>
#include <v8.h>

namespace rootJS
{
	/**
	 * Maps C++ strings and c-strings to JavaScript strings.
	 */
	class StringProxy: public PrimitiveProxy
	{
		private:
			enum class StringType {
			    CHAR, STRING, TSTRING
			};

			/**
			 * Returns a c_string
			 * If this is based on a char ptr, this ptr will be returned
			 * If this is based on a std::string its c_str will be returned
			 */
			const char* c_str();

			/**
			 * Enum value representing the type
			 */
			StringType strType;

		public:
			/**
			 * Create a new StringProxy.
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			StringProxy(MetaInfo &info, TClass *scope);

			/**
			 * Check if the type is a boolean type.
			 *
			 * @param type the type to be checked
			 * @return if the type is a string type.
			 */
			static bool isString(std::string type);

			/**
			 * Creates a StringProxy based on a const char*, nullterminated string
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy* charConstruct(MetaInfo &info, TClass *scope);

			/**
			 * Creates a StringProxy based on a const std::string, nullterminated string
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy* stringConstruct(MetaInfo &info, TClass *scope);

			/**
			 * Creates a StringProxy based on a const TString, nullterminated string
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy* tStringConstruct(MetaInfo &info, TClass *scope);


			/**
			 * Returns a v8 String
			 * Copies the c_String which is used to power the Object
			 * represented by the MetaInfo object
			 */
			virtual v8::Local<v8::Value> get();

			/**
			 * When the base is an immutable string (std::String, TString) this
			 * will set a new value
			 */
			virtual void setValue(v8::Local<v8::Value> value);
	};
}

#endif
