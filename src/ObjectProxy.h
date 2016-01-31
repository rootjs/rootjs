#ifndef SRC_OBJECTPROXY_H_
#define SRC_OBJECTPROXY_H_

#include "Proxy.h"
#include "ProxyMode.h"
#include "MemberMode.h"
#include "GlobalMode.h"
#include <v8.h>

#include <TDataMember.h>
#include <TClassRef.h>
#include <TGlobal.h>
#include <RConfig.h>

namespace rootJS {

	/**
	 *The ObjectProxy class is used to represent ROOT objects.
	 *It differentiates between primitive and non-primitive object types.
	 */
	class ObjectProxy : public Proxy {

		public:
			/**
			 * Create a new ObjectProxy of a TObject.
			 *
			 * @param type
			 * 			the type of the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			ObjectProxy(ProxyMode &type, TClassRef scope);;

			/*
			 * Gets the address of the encapsulated object
			 */

			virtual ~ObjectProxy();

			/**
			 * Return the name of the type
			 * @return the name of the type
			 */
			const char* getTypeName();

			/*
			*Returns an object encapsulating meta
			*/
			ProxyMode *getTypeInfo();

			/*
			*get the offset
			@return the offset
			*/
			Long_t getOffset();

			/**
			 * Assign the specified value to this ObjectProxy.
			 *
			 * @param value
			 * 			the value to assign to this ObjectProxy
			 */
			virtual void set(ObjectProxy &value);

			/**
			 * Return the encapsulating javascript value.
			 *
			 * @return the encapsulating javascript value
			 */
			virtual v8::Local<v8::Value> get();

			/**
			 * Set the encapsulating javascript object.
			 *
			 * @param proxy
			 * 			the encapsulating javascript object
			 *
			 */
			virtual void setProxy(v8::Local<v8::Object> proxy);

			/**
			 * Return the encapsulating javascript object.
			 *
			 * @return the encapsulating javascript object
			 */
			virtual v8::Local<v8::Object> getProxy();

			virtual void setValue(v8::Local<v8::Value> value);

			/**
			 * Check if this proxy encapsulates a primitive type.
			 *
			 * @return true if this ProxyObject encapsulates a primitive data type
			 */
			virtual bool isPrimitive();

			virtual bool isTemplate();
			virtual bool isGlobal();
			virtual bool isConst();
			virtual bool isStatic();

		protected:
			v8::Persistent<v8::Object> proxy; /**< the exposed javascript object */
		private:
			ProxyMode *currentmode;
	};
}

#endif /* SRC_OBJECTPROXY_H_ */
