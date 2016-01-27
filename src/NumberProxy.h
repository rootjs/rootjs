#ifndef NUMBER_PROXY_H
#define NUMBER_PROXY_H

#include "ObjectProxy.h"
#include "PrimitiveProxy.h"

#include <string>

#include <TDataMember.h>
#include <TClassRef.h>

#include <v8.h>

namespace rootJS {
	enum class NumberType {
		INT_T, DOUBLE_T, SHORT_T, USHORT_T, UINT_T,  LONG_T, ULONG_T, FLOAT_T,
		LONG64_T, ULONG64_T, LONGDOUBLE_T
	};


	class NumberProxy: public PrimitiveProxy {
	private:
		NumberType numberType;

		Double_t castToDouble(void*);
	public:
		/**
		 * Check if the type is a number type.
		 *
		 * @param type the type to be checked
		 * @return if the type is a number type
		 */
		static bool isNumber(std::string type);

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
		//static ObjectProxy* intConstruct(const TDataMember& type, TClassRef scope);

#define ROOTJS_NUMBER_PROXY_DECLARE( datatype )                   \
        static ObjectProxy* datatype##Construct(const TDataMember& type, TClassRef scope);

#define ROOTJS_NUMBER_PROXY_DECLARE_2( datatype )                   \
        static ObjectProxy* datatype##Construct(void *address ,const TGlobal& type, TClassRef scope);   \

		ROOTJS_NUMBER_PROXY_DECLARE(int);
		ROOTJS_NUMBER_PROXY_DECLARE_2(int);

		ROOTJS_NUMBER_PROXY_DECLARE(double);
		ROOTJS_NUMBER_PROXY_DECLARE_2(double);

		ROOTJS_NUMBER_PROXY_DECLARE(short);
		ROOTJS_NUMBER_PROXY_DECLARE(ushort);

		ROOTJS_NUMBER_PROXY_DECLARE_2(short);
		ROOTJS_NUMBER_PROXY_DECLARE_2(ushort);


		ROOTJS_NUMBER_PROXY_DECLARE(uint);

		ROOTJS_NUMBER_PROXY_DECLARE(float);

		ROOTJS_NUMBER_PROXY_DECLARE(ldouble);

		ROOTJS_NUMBER_PROXY_DECLARE(long);
		ROOTJS_NUMBER_PROXY_DECLARE(ulong);

		ROOTJS_NUMBER_PROXY_DECLARE(llong);
		ROOTJS_NUMBER_PROXY_DECLARE(ullong);

		ROOTJS_NUMBER_PROXY_DECLARE(_int64);
		ROOTJS_NUMBER_PROXY_DECLARE(u_int64);



		ROOTJS_NUMBER_PROXY_DECLARE_2(uint);

		ROOTJS_NUMBER_PROXY_DECLARE_2(float);

		ROOTJS_NUMBER_PROXY_DECLARE_2(ldouble);

		ROOTJS_NUMBER_PROXY_DECLARE_2(long);
		ROOTJS_NUMBER_PROXY_DECLARE_2(ulong);

		ROOTJS_NUMBER_PROXY_DECLARE_2(llong);
		ROOTJS_NUMBER_PROXY_DECLARE_2(ullong);


		ROOTJS_NUMBER_PROXY_DECLARE_2(_int64);
		ROOTJS_NUMBER_PROXY_DECLARE_2(u_int64);


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
		//static ObjectProxy* intConstruct(void *address, const TGlobal& type, TClassRef scope);

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
		//static ObjectProxy* doubleConstruct(const TDataMember& type, TClassRef scope);

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
		//static ObjectProxy* doubleConstruct(void *address, const TGlobal& type, TClassRef scope);

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
