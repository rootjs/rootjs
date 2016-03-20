#ifndef H_OBJECT_PROXY_PROTOTYPE
#define H_OBJECT_PROXY_PROTOTYPE

#include "MetaInfo.h"
#include "ObjectProxy.h"

#include <vector>

#include <nan.h>

#include <TClass.h>

namespace rootJS {
	/**
	 * The ObjectProxyBuilder is used to prepare an object proxy.
	 * It shall not use v8 in any way before createObjectProxy has been called,
	 * therefore it can be used in async calls.
	 */
	class ObjectProxyBuilder {
	public:
		/**
		 * Creates a new ObjectProxyBuilder
		 */
		ObjectProxyBuilder();

		/**
		 * Destroys this ObjectProxyBuilder
		 */
		~ObjectProxyBuilder();

		/*
		 * Copies the passed object proxy
		 * @param o object to be copied
		 */
		ObjectProxyBuilder(const ObjectProxyBuilder &o);

		/*
		 * Swaps the passed ObjectProxyBuilder into the ObjectProxyBuilder
		 * on the left
		 */
		ObjectProxyBuilder& operator=(const ObjectProxyBuilder &o);

		/*
		 * Sets the base instance which is generated during the initial,
		 * encapsulation.
		 * @instance v8 instance to use in the ObjectProxyFactory
		 */
		void setBaseInstance(v8::Local<v8::Object> instance);

		/**
		 * Sets the meta info of a function return
		 * @param info the meta info to use inside the ObjectProxFactory
		 */
		void setResultInfo(MetaInfo &info);

		/**
		 * Sets a class ref
		 * @param clazz the classref to use in the CallbackHandlerFactory
		 */
		void setClass(TClass *clazz);

		/**
		 * Adds a pointer that should be freed when the ObjectProxy is being
		 * destroyed.
		 */
		void bindAllocatedMemory(void *);

		/**
		 * Checks if a call to createObjectProxy is possible with the current info
		 *
		 * @return true when a MetaInfo is available
		 */
		bool isValid();

		/**
		 * Creates an objectProxy based on the stored builder information
		 * @return the corresponding objectProxy, or a nullpointer if no
		 * matching ObjectProxy could be found
		 */
		ObjectProxy *createObjectProxy();
	private:
		std::vector<void*> boundAllocatedMemory;
		Nan::Persistent<v8::Object> instance;
		MetaInfo *info = nullptr;
		TClassRef *clazz = nullptr;
	};
}

#endif /*H_OBJECT_PROXY_PROTOTYPE*/
