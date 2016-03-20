#ifndef ENUMCONSTPROXY_H_
#define ENUMCONSTPROXY_H_

#include "PrimitiveProxy.h"
#include <TEnumConstant.h>

namespace rootJS
{

	class EnumConstProxy : public PrimitiveProxy
	{

		public:
			static TEnumConstant* fromValue(v8::Local<v8::Value> value, std::string typeName);
			static TEnumConstant* fromNumber(v8::Local<v8::Number> number, std::string typeName);

			EnumConstProxy(MetaInfo &info, TClass *scope);

			virtual ~EnumConstProxy();

			/**
			 * Return a v8::Number containing the value of the proxied TEnumConstant.
			 *
			 * @return a v8::Number containing the value of the proxied TEnumConstant
			 */
			virtual v8::Local<v8::Value> get();

			/**
			 * Assign another TEnumConstant (of the corporate TEnum) to this proxy.
			 *
			 * @param value
			 *              the enum constant to assign
			 */
			virtual void setValue(v8::Local<v8::Value> value);
	};

}

#endif
