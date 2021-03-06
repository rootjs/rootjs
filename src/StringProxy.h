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
			char singleChar[2];
			/**
			 * An enum containing the two standard string types, string and c-strings.
			 * It also contains standard string type in ROOT, TStrings.
			 */
			enum class StringType {
				CHAR, STRING, TSTRING, SINGLE_CHAR
			};

			/**
			 * Returns a c_string
			 * @return  If this is based on a char ptr, this ptr will be returned
			 *          If this is based on a std::string its c_str will be returned
			 */
			const char* c_str();


			StringType strType; /** Enum value representing the type */
		public:
			/**
			 * Create a new StringProxy.
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			StringProxy(MetaInfo &info, TClass *scope);


			~StringProxy();

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
			 * Creates a StringProxy based on a single char.
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy* singleCharConstruct(MetaInfo &info, TClass *scope);


			/**
			 * Returns a v8 String
			 * Copies the c_String which is used to power the Object
			 * represented by the MetaInfo object
			 */
			virtual v8::Local<v8::Value> get();

			/**
			 * When the base is an immutable string (std::String, TString) this
			 * will set a new value
			 * @param value
			 *              The value to be set
			 */
			virtual void setValue(v8::Local<v8::Value> value);
	};
}

#endif // STRING_PROXY_H
