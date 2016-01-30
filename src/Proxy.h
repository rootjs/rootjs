#ifndef PROXY_H_
#define PROXY_H_

#include "TObject.h"
#include "TClassRef.h"

namespace rootJS {
	/**
	 * The proxy super class from which both proxies inherit.
	 * The proxies act as intermediary between Node.js and ROOT.
	 */
	class Proxy {
		private:

		protected:
			Proxy(void* address, const TObject & type, TClassRef & scope);
			virtual ~Proxy();

			void* address; /**<adress of encapsulated object in memory*/
			const TObject& type; /**<type meta information of encapsulated object */
			TClassRef& scope; /**<scope meta information of encapsulated object*/

		public:

			/**
			 * set the address this proxy points to
			 * @param address the new address
			 */
			virtual void setAddress(void* address);

			/**
			 * get the address of the encapsulated object
			 * @return the encapsulated object's address
			 */
			virtual void* getAddress();

			/**
			 * get meta information about the encapsulated objcet's scope
			 * @return meta information about the scope
			 */
			TClassRef& getScope();

			/**
			 * get meta information about the encapsulated objcet's type
			 * @return meta information about the type
			 */
			virtual const TObject& getType();

			/**
			 * check if the encapsulated object is a template
			 * @return if the encapsulated object is a template
			 */
			virtual bool isTemplate() = 0;

			/**
			 * check if the encapsulated object is global
			 * @return if the encapsulated object is global
			 */
			virtual bool isGlobal() = 0;

			/**
			 * check if the encapsulated object is constant
			 * @return if the encapsulated object is constant
			 */
			virtual bool isConst() = 0;

			/**
			 * check if the encapsulated object is static
			 * @return if the encapsulated object is static
			 */
			virtual bool isStatic() = 0;
	};
}
#endif /* PROXY_H_ */
