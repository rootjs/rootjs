#ifndef NUMBER_PROXY_H
#define NUMBER_PROXY_H

#include "ObjectProxy.h"
#include "PrimitiveProxy.h"

#include <TDataMember.h>
#include <TClassRef.h>

#include <v8.h>

namespace RootJS {
	enum class NumberType {
		INT_T, DOUBLE_T
	};


	class NumberProxy: public PrimitiveProxy {
	private:
		NumberType numberType;

		Double_t castToDouble(void*);
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
		static ObjectProxy* intConstruct(const TDataMember& type, TClassRef scope);

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
		static ObjectProxy* intConstruct(void *address, const TGlobal& type, TClassRef scope);

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
		static ObjectProxy* doubleConstruct(const TDataMember& type, TClassRef scope);

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
		static ObjectProxy* doubleConstruct(void *address, const TGlobal& type, TClassRef scope);

		/**
		 * Return the encapsulating javascript value.
		 *
		 * @return the encapsulating javascript value
		 */
		virtual v8::Local<v8::Value> get();

		/**
		 * Setter for v8 values, writes new data to memory
		 * @param value
		 * 			the value set via node, to be stored at the memory address
		 */
		virtual void setValue(v8::Local<v8::Value> value);
	};
}

#endif /* NUMBER_PROXY_H */
