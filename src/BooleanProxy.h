#ifndef BOOLEAN_PROXY_H
#define BOOLEAN_PROXY_H

#include "PrimitiveProxy.h"

#include <string>
#include <regex>

namespace rootJS
{
	/**
	 * Maps a C++/ROOT boolean to JavaScript boolean.
	 */
	class BooleanProxy: public PrimitiveProxy
	{
		public:
			/**
			 * Check if the type is a boolean type.
			 *
			 * @param type the type to be checked
			 * @return if the type is a boolean type
			 */
			static bool isBoolean(std::string type);

			/**
			 * Create a new BooleanProxy.
			 *
			 * @param info the type of the excapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			BooleanProxy(MetaInfo &info, TClass *scope);

			~BooleanProxy();

			/**
			 * Creates a BooleanProxy, can be derefered to be added to a map
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy* boolConstruct(MetaInfo &info, TClass *scope);

			/**
			 * Returns a v8 Boolean depending on MetaInfo
			 *
			 * @return boolean depending on MetaInfo
			 */
			virtual v8::Local<v8::Value> get();

			/**
			 * Sets the boolen in memory, using the data passed via JavaScript
			 *
			 * @param value
			 *              The value to be set
			 */
			virtual void setValue(v8::Local<v8::Value> value);
	};
}

#endif
