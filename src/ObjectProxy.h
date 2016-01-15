#ifndef SRC_OBJECTPROXY_H_
#define SRC_OBJECTPROXY_H_

#include "v8.h"

#include "TDataMember.h"
#include "TClassRef.h"

#include "Proxy.h"

namespace RootJS {

/**
 *The ObjectProxy class is used to represent ROOT objects.
 *It differentiates between primitive and non-primitive object types.
 */
class ObjectProxy: public Proxy {

public:

	/**
	 * Create a new ObjectProxy.
	 *
	 * @param type
	 * 			the type of the encapsulated object
	 *
	 * @param scope
	 *			the scope of the encapsulated object
	 */
	ObjectProxy(TDataMember& type, TClassRef& scope);

	virtual ~ObjectProxy();

	/**
	 * Return the meta information about the type of the encapsulated object.
	 *
	 * @return the meta information about the type of the encapsulated object.
	 */
	TDataMember& getType();

	/**
	 * Assign the specified value to this ObjectProxy.
	 *
	 * @param value
	 * 			the value to assign to this ObjectProxy
	 */
	virtual void set(ObjectProxy const& value);

	/**
	 * Return the encapsulating javascript object.
	 *
	 * @return the encapsulating javascript object
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

protected:
	v8::Persistent<v8::Object> proxy; /**< the exposed javascript object */

};

}

#endif /* SRC_OBJECTPROXY_H_ */
