#include "ObjectProxyBuilder.h"

#include "ObjectProxyFactory.h"

#include <assert.h>

namespace rootJS {
	ObjectProxyBuilder::ObjectProxyBuilder() {

	}

	ObjectProxyBuilder::ObjectProxyBuilder(const ObjectProxyBuilder &o) {
		if(o.info != nullptr) {
			this->info = o.info->clone();
		}
		if(o.clazz != nullptr) {
			this->clazz = new TClassRef(*(o.clazz));
		}
	}

	ObjectProxyBuilder& ObjectProxyBuilder::operator=(const ObjectProxyBuilder &o) {
		boundAllocatedMemory = o.boundAllocatedMemory;
		instance = o.instance;

		if(o.info != nullptr) {
			this->info = o.info->clone();
		}
		if(o.clazz != nullptr) {
			this->clazz = new TClassRef(*(o.clazz));
		}

    	return *this;
	}

	ObjectProxyBuilder::~ObjectProxyBuilder() {
		if(info != nullptr) {
			delete info;
		}
		if(clazz != nullptr) {
			delete clazz;
		}
		if(!instance.IsEmpty() && !(instance.IsWeak() || instance.IsNearDeath())) {
			instance.Reset();
		}
	}

	void ObjectProxyBuilder::setBaseInstance(v8::Local<v8::Object> instance) {
		this->instance.Reset(v8::Isolate::GetCurrent(), instance);
	}

	void ObjectProxyBuilder::setResultInfo(MetaInfo &info) {
		this->info = info.clone();
	}

	void ObjectProxyBuilder::setClass(TClass *clazz) {
		if(this->clazz != nullptr) {
			delete this->clazz;
		}
		this->clazz = new TClassRef(clazz);
	}

	void ObjectProxyBuilder::bindAllocatedMemory(void *ptr) {
		this->boundAllocatedMemory.push_back(ptr);
	}

	bool ObjectProxyBuilder::isValid() {
		return info != nullptr;
	}

	ObjectProxy *ObjectProxyBuilder::createObjectProxy() {
		assert(isValid());
		TClass *clazz = nullptr;
		if(this->clazz != nullptr) {
			clazz = this->clazz->GetClass();
		}

		ObjectProxy *proxy;
		if(instance.IsEmpty()) {
			proxy = ObjectProxyFactory::createObjectProxy(*info, clazz);
		} else {
			v8::Local<v8::Object> localInstance
				= v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
			proxy = ObjectProxyFactory::createObjectProxy(*info, clazz, localInstance);
		}

		for (std::vector<void*>::iterator it = boundAllocatedMemory.begin() ; it != boundAllocatedMemory.end(); ++it) {
			proxy->registerMallocedSpace(*it);
		}
		return proxy;
	}
}
