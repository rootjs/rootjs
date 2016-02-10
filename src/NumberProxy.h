#ifndef NUMBER_PROXY_H
#define NUMBER_PROXY_H

#include "ObjectProxy.h"
#include "PrimitiveProxy.h"

#include <v8.h>
#include <string>

#include "TClass.h"

namespace rootJS
{
    /**
     * An enum containing the standard number types in C++.
     */
	enum class NumberType
	{
	    INT_T, DOUBLE_T, SHORT_T, USHORT_T, UINT_T,  LONG_T, ULONG_T, FLOAT_T,
	    LONG64_T, ULONG64_T, LONGDOUBLE_T
	};

	/**
     * The NumberProxy is the proxy between C++ numbers and JavaScript number.
     * The NumberProxy uses a C++ macro to map all C++ numbers to JavaScript numbers,
     * and all number are casted to doubles, as doubles are the number type
     * supported by JavaScipt.
     */
	class NumberProxy: public PrimitiveProxy
	{
		private:
			NumberType numberType; /** The type of the C++/ROOT number */

            /**
             * As JavaScript can only support doubles, all number types
             * have to be cast to a double.
             *
             * @param address of the number which is to be cast
             * @return the number casted into a double
             */
			Double_t castToDouble(void*);
			bool backedUp = false;
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
			 * @param info
			 * 			the type of the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			NumberProxy(MetaInfo &info, TClass *scope);

			~NumberProxy();

            /**
             * Saves the value to the heap
             */
			virtual void backup();

#define ROOTJS_NUMBER_PROXY_DECLARE(datatype)                   \
            static ObjectProxy* datatype##Construct(MetaInfo &info, TClass *scope);


			///@{
			/**
			* This calls the constructor.
			* We cannot create pointers to constructors,
			* but need to map the constructors in an Factory.
			* This is a macro to declare the constructors for the
			* various number types.
			*
			* @param type
			* 			the type of the encapsulated object
			*
			* @param scope
			*			the scope of the encapsulated object
			*/
			ROOTJS_NUMBER_PROXY_DECLARE(int);
			ROOTJS_NUMBER_PROXY_DECLARE(uint);

			ROOTJS_NUMBER_PROXY_DECLARE(short);
			ROOTJS_NUMBER_PROXY_DECLARE(ushort);

			ROOTJS_NUMBER_PROXY_DECLARE(float);

			ROOTJS_NUMBER_PROXY_DECLARE(double);
			ROOTJS_NUMBER_PROXY_DECLARE(ldouble);

			ROOTJS_NUMBER_PROXY_DECLARE(long);
			ROOTJS_NUMBER_PROXY_DECLARE(ulong);

			ROOTJS_NUMBER_PROXY_DECLARE(llong);
			ROOTJS_NUMBER_PROXY_DECLARE(ullong);

			ROOTJS_NUMBER_PROXY_DECLARE(_int64);
			ROOTJS_NUMBER_PROXY_DECLARE(u_int64);
			///@}


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
