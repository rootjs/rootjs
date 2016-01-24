#ifndef NUMBER_PROXY_H
#define NUMBER_PROXY_H

#include "ObjectProxy.h"
#include "PrimitiveProxy.h"

#include <TDataMember.h>
#include <TClassRef.h>

namespace RootJS {
	class NumberProxy: public PrimitiveProxy {
	public:
		/**
		 * Create a new NumberProxy.
		 *
		 * @param type
		 * 			the type of the encapsulated object
		 *
		 * @param scope
		 *			the scope of the encapsulated object
		 */
		NumberProxy(const TDataMember& type, TClassRef scope);

		/**
		 * Creates a new ObjectProxy.
		 * Use this constructor when you do not have a TDataMember
		 * (This happens when you want to proxy an object which is globally available)
		 *
		 * @param object
		 *      A pointer to the object that should be prxied.
		 *      We need a pointer to get the correct address.
		 */
		NumberProxy(void *object, const TGlobal & type, TClassRef scope);

		/**
		 * This calls the constructor.
		 * We cannot create pointers to constructors,
		 * but need to map the constructors in out Factory.

		 * @param type
		 * 			the type of the encapsulated object
		 *
		 * @param scope
		 *			the scope of the encapsulated object
		 */
		static ObjectProxy* construct(const TDataMember& type, TClassRef scope);

		/**
		 * This calls the constructor.
		 * We cannot create pointers to constructors,
		 * but need to map the constructors in out Factory.

		 * @param address
		 * 		  the address of the global
		 *
		 * @param type
		 * 			the type of the encapsulated object
		 *
		 * @param scope
		 *			the scope of the encapsulated object
		 */
		static ObjectProxy* construct(void *address, const TGlobal& type, TClassRef scope);

		/**
		 * Return the encapsulating javascript value.
		 *
		 * @return the encapsulating javascript value
		 */
		virtual v8::Local<v8::Value> get();
	};
}

#endif /* NUMBER_PROXY_H */
