#ifndef SRC_OBJECTPROXY_H_
#define SRC_OBJECTPROXY_H_

#include "Proxy.h"
#include "MetaInfo.h"
#include "MemberInfo.h"
#include "GlobalInfo.h"

#include <v8.h>

#include "TDataMember.h"
#include "TClass.h"
#include "TClassRef.h"
#include "TGlobal.h"
#include "RConfig.h"


namespace rootJS
{

	/**
	 * The ObjectProxy class is used to represent ROOT objects.
	 * It differentiates between primitive and non-primitive object types.
	 */
	class ObjectProxy : public Proxy
	{

		public:
			/**
			 * Create a new ObjectProxy of a TObject.
			 *
			 * @param info
			 * 			the type of the encapsulated object
			 *
			 * @param scope
			 *			the scope of the encapsulated object
			 */
			ObjectProxy(MetaInfo &info, TClass *scope);

			~ObjectProxy();

			/**
			 * Return the name of the type
			 * @return the name of the type
			 */
			const char* getTypeName();

			/**
			* Get the offset
			* @return the offset
			*/
			Long_t getOffset();

			/**
			 * Assign the specified value to this ObjectProxy.
			 *
			 * @param value
			 * 			the value to assign to this ObjectProxy
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

			/**
			 * Check if this proxy encapsulates a template.
			 *
			 * @return true if this ProxyObject encapsulates a template
			 */
			virtual bool isTemplate();

			/**
			 * Check if this proxy encapsulates a global.
			 *
			 * @return true if this ProxyObject encapsulates a global
			 */
			virtual bool isGlobal();
			/**
			 * Check if this proxy encapsulates a constant.
			 *
			 * @return true if this ProxyObject encapsulates a constant
			 */
			virtual bool isConst();
			/**
			 * Check if this proxy encapsulates a static.
			 *
			 * @return true if this ProxyObject encapsulates a static
			 */
			virtual bool isStatic();

			void registerMallocedSpace(void*);

			v8::Persistent<v8::Object> &getWeakPeristent();

		protected:
			v8::Persistent<v8::Object> proxy; /**< the exposed javascript object */

		private:
			std::vector<void*> boundMallocs;
			bool isWeak = false;
			static void weakCallback(v8::WeakCallbackData<v8::Object, ObjectProxy> const& data);
	};
}

#endif
