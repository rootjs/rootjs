#ifndef VOIDPOINTERPROXY_H_
#define VOIDPOINTERPROXY_H_

#include "PrimitiveProxy.h"

namespace rootJS
{

	class VoidPointerProxy : public PrimitiveProxy
	{
		public:
			/**
			 * Create a new VoidPointerProxy.
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			VoidPointerProxy(MetaInfo &info, TClass *scope);

			virtual ~VoidPointerProxy();

			/**
			 * Return a v8::String containing the address the proxied void pointer points to.
			 *
			 * @return a string containing the address the proxied void pointer points to.
			 */
			virtual v8::Local<v8::Value> get();

			/**
			 * Set the address to point to.
			 * NOTE: changing the pointer directly is not allowed yet.
			 *
			 * @param value
			 *              the address to point to
			 */
			virtual void setValue(v8::Local<v8::Value> value);

			/**
			 * Create a new VoidPointerProxy.
			 *
			 * @param info the type of the encapsulated object
			 * @param scope the scope of the encapsulated object
			 */
			static ObjectProxy* voidConstruct(MetaInfo &info, TClass *scope);

			virtual bool isConst();
	};

}

#endif
