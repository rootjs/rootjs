#include "ObjectProxyFactory.h"
#include "GlobalMode.h"
#include "MemberMode.h"

#include "Proxy.h"
#include <iostream>
#include <map>
#include <v8.h>
#include <TClass.h>
#include <TClassRef.h>
#include <TClassTable.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TList.h>

/* Includes for proxyMap */
#include "NumberProxy.h"

namespace rootJS {

	std::map<std::string, ProxyInitializator> ObjectProxyFactory::proxyMap;

	void ObjectProxyFactory::traverseClass(TClassRef & classRef, ObjectProxy & proxy) {
		TClass *klass = classRef.GetClass();

		TList *propertyList = klass->GetListOfAllPublicDataMembers();
		TIter nextProperty(propertyList);
		TDataMember *member;

		while ((member = (TDataMember*)nextProperty())) {
			v8::Local<v8::Object> nodeObject = proxy.getProxy();
			ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(*member, classRef, proxy);
			nodeObject->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName()), memberProxy->get());
		}
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(TGlobal & object) {
		if(!object.IsValid() || !object.GetAddress()) {
			return nullptr;
		}
		GlobalMode gMode(object);
		ObjectProxy* nonObjectProxy = determineProxy(gMode, TClassRef());

		if(nonObjectProxy) {
			return nonObjectProxy;
		}


		const char *className = getClassNameFromType(object.GetTypeName());
		DictFuncPtr_t dictFunc = gClassTable->GetDict(className);
		if(dictFunc == nullptr) {
			return nullptr;
		}
		TClass *klass = dictFunc();

		TClassRef classRef = TClassRef(klass);
		GlobalMode mode(object);
		ObjectProxy *proxy = new ObjectProxy(mode, classRef);
		//Set an empty proxy and fill iit in the following loops
		proxy->setProxy(v8::Object::New(v8::Isolate::GetCurrent()));

		traverseClass(classRef, *proxy);

		return proxy;
	}

	const char* ObjectProxyFactory::getClassNameFromType(const char* type) {
		std::string typeString = std::string(type);
		std::string className = typeString.substr(0, typeString.length()-1);
		return className.c_str();
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(const TDataMember & type, TClassRef scope, ObjectProxy & holder) {
		/*
		 * It is not possible to do pointer arithmetic on void pointers.
		 * To add the offset to the object we cast the pointer to char* before.
		 */
		void *object = static_cast<void*>(
		                   (static_cast<char*>(holder.getAddress()) + type.GetOffsetCint())
		               );

		MemberMode mode(type, object);

		ObjectProxy *memberProxy = determineProxy(mode, scope);
		if(memberProxy) {
			memberProxy->setAddress(object);
		} else {
			//TODO object?
			MemberMode mode(type, object);
			memberProxy = new ObjectProxy(mode, scope);
			memberProxy->setProxy(v8::Object::New(v8::Isolate::GetCurrent()));
		}

		return memberProxy;
	}

	void ObjectProxyFactory::createObjectProxy(void* address, TClassRef &type, v8::Local<v8::Object> proxy) {
		// TODO
	}

	ObjectProxy* ObjectProxyFactory::determineProxy(ProxyMode& type, TClassRef ref) {
		std::string typeString = std::string(type.getTypeName());
		if(proxyMap.find(typeString) == proxyMap.end()) {
			return nullptr;
		}

		return proxyMap[typeString](type, ref);
	}

	void ObjectProxyFactory::initializeProxyMap() {
		//Int_t
		proxyMap["Int_t"] = &NumberProxy::intConstruct;

		//Double_t
		proxyMap["Double_t"] = &NumberProxy::doubleConstruct;
	}

}
