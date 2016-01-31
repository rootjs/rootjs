#ifndef SRC_OBJECTPROXY_H_
#define SRC_OBJECTPROXY_H_

#include "Proxy.h"
#include <v8.h>
#include "MetaInfo.h"

namespace rootJS
{

	/**
	 *The ObjectProxy class is used to represent ROOT objects.
	 *It differentiates between primitive and non-primitive object types.
	 */
	class ObjectProxy : public Proxy
	{

		public:

			/**
			 * Create a new ObjectProxy.
			 *
			 * @param address
			 * 			the memory address of the encapsulated object
			 *
			 * @param info
			 * 			the meta info about the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			ObjectProxy(void* address, MetaInfo *info, TClass* scope);

			/**
			 * Create a new ObjectProxy.
			 *
			 * @param info
			 * 			the meta info about the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			ObjectProxy(MetaInfo *info, TClass* scope);

			virtual ~ObjectProxy();

			/*
			 * Gets the address of the encapsulated object
			 */
			virtual void* getAddress();


			/*
			* Returns an object encapsulating meta
			*
			* @return
			*/
			MetaInfo* getMetaInfo();

			/**
			 * Assign the specified value to this ObjectProxy.
			 *
			 * @param value
			 * 			the value to assign to this ObjectProxy
			 */
			virtual void set(ObjectProxy &value);

			/*
			 * TODO remove
			 */
			virtual void setValue(v8::Local<v8::Value> value);

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
			MetaInfo *info;
	};
}

#endif
