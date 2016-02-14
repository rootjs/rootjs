#include "ObjectProxy.h"

#include "Toolbox.h"

#include <TObject.h>
#include <TGlobal.h>
#include <TClassTable.h>

namespace rootJS
{
	std::map<TObject*, ObjectProxy*> ObjectProxy::objMap;

	ObjectProxy::ObjectProxy(MetaInfo &info, TClass *scope) : Proxy(info, scope)
	{
	}

	ObjectProxy::~ObjectProxy()
	{
		if(isWeak) {
			DictFuncPtr_t dictPtr = gClassTable->GetDict(getTypeName());
			if(dictPtr != nullptr) {
				TClass *klass = dictPtr();
				TClass *objClass = klass->GetBaseClass("TObject");
				if(objClass) {
					TObject *objPtr = *(TObject**)getAddress();
					klass->Destructor(*(void**)getAddress(), true);
					objMap.erase(objPtr);
				} else {
					dictPtr()->Destructor(*(void**)getAddress(), true);
				}
			}
		}

		for(void* ptr : boundMallocs) {
			free(ptr);
		}
	}

	void ObjectProxy::weakCallback(v8::WeakCallbackData<v8::Object, ObjectProxy> const& data)
	{
		ObjectProxy *objectProxy = data.GetParameter();
		objectProxy->proxy.Reset();

		delete objectProxy;
	}

	const char* ObjectProxy::getTypeName()
	{
		return getTypeInfo()->getTypeName();
	}

	Long_t ObjectProxy::getOffset()
	{
		return getTypeInfo()->getOffset();
	}

	v8::Local<v8::Value> ObjectProxy::get()
	{
		// objects just return their holder - i.e the proxy member
		return getProxy();
	}

	void ObjectProxy::setValue(v8::Local<v8::Value> value)
	{
		return;
	}

	void ObjectProxy::setProxy(v8::Local<v8::Object> proxy)
	{
		this->proxy.Reset(v8::Isolate::GetCurrent(), proxy);
	}

	v8::Local<v8::Object> ObjectProxy::getProxy()
	{
		return v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(), proxy);
	}

	bool ObjectProxy::isPrimitive()
	{
		return false;
	}

	bool ObjectProxy::isTemplate()
	{
		return false;
	}

	bool ObjectProxy::isGlobal()
	{
		return getTypeInfo()->isGlobal();
	}

	bool ObjectProxy::isConst()
	{
		return getTypeInfo()->isConst();
	}

	bool ObjectProxy::isStatic()
	{
		return getTypeInfo()->isStatic();
	}

	void ObjectProxy::registerMallocedSpace(void *allocated)
	{
		boundMallocs.push_back(allocated);
	}

	v8::Persistent<v8::Object> &ObjectProxy::getWeakPeristent()
	{
		proxy.SetWeak(this, weakCallback);
		DictFuncPtr_t dictPtr = gClassTable->GetDict(getTypeName());
		if(dictPtr != nullptr) {
			TClass *klass = dictPtr();
			TClass *objClass = klass->GetBaseClass("TObject");
			if(objClass) {
				objMap[*(TObject**)getAddress()] = this;
			}
		}
		isWeak = true;
		return proxy;
	}

	void ObjectProxy::removed() {
		isWeak = false;
		proxy.Reset();
	}

	void ObjectProxy::rootDesturcted(TObject* obj) {
		if(objMap.find(obj) != objMap.end()) {
			objMap[obj]->removed();
			//delete objMap[obj];
			objMap.erase(obj);
		}
	}
}
