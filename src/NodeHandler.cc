#include "NodeHandler.h"
#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "CallbackHandler.h"
#include "NodeApplication.h"
#include "TemplateFactory.h"

#include <TROOT.h>
namespace rootJS {

NodeHandler *NodeHandler::instance;
bool NodeHandler::initialized;
void NodeHandler::exposeGlobalFunctions() {
}
void NodeHandler::exposeMacros() {
}

void NodeHandler::exposeClasses() {
	std::map<TClass*, v8::Local<v8::ObjectTemplate>> exposedNamespaces;
	TCollection* classes = gROOT->GetListOfClasses();
	TIter next(classes);
	//iterate over all classes
	while (TClass *clazz = (TClass*) next()) {
		//get full name of class and split it by "::" to get the namespaces the class resides in
		std::string splitname = (clazz->GetName());
		std::queue<std::string> que = splitClassName(splitname);
		std::string curname = que.front();

		//if it is in the top namespace just create a template and set it in exports
		if ((que.size() == 1)) {
			if (clazz->Property() & kIsClass) {
				this->exports->Set(
						v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
								curname.c_str()),
						TemplateFactory::createTemplate(TClassRef(clazz))->GetFunction());
			}
			if (clazz->Property() & kIsNamespace) {
				this->exports->Set(
						v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
								curname.c_str()),
						createNamespace(clazz, exposedNamespaces)->NewInstance());
			}
		} else {
			//only process classes and namespaces
			if ((clazz->Property() & kIsNamespace)
					|| (clazz->Property() & kIsClass)) {

				DictFuncPtr_t dickFunc = gClassTable->GetDict(curname.c_str());
				TClass* recclass = dickFunc();
					//TODO exception magic

				this->exports->Set(
						v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
								curname.c_str()),
						exposeClassRec(recclass, que, exposedNamespaces)->NewInstance());
			}
		}
	}
}

v8::Local<v8::ObjectTemplate> NodeHandler::exposeClassRec(TClass* curclass,
		std::queue<std::string>& que,
		std::map<TClass*, v8::Local<v8::ObjectTemplate>>& exposedNamespaces) {
	if (que.empty()) {
		return createNamespace(curclass, exposedNamespaces);
	}

	que.pop();
	DictFuncPtr_t dickFunc = gClassTable->GetDict(que.front().c_str());
	TClass* recclass = dickFunc();
	v8::Local<v8::ObjectTemplate>  curtmpl = createNamespace(curclass, exposedNamespaces);
	if (recclass->Property() & kIsNamespace) {
		v8::Local<v8::ObjectTemplate> rectmpl = exposeClassRec(recclass, que,
				exposedNamespaces);
		curtmpl->Set(v8::Isolate::GetCurrent(), curclass->GetName(),
				rectmpl->NewInstance());
	}
	return curtmpl;

}

void NodeHandler::initialize(v8::Local<v8::Object> exports,
		v8::Local<v8::Object> module) {
	if (!initialized) {
		NodeApplication::CreateNodeApplication();
		ObjectProxyFactory::initializeProxyMap();
		instance = new NodeHandler(exports);
		instance->exposeROOT();
	} else {
		v8::Isolate::GetCurrent()->ThrowException(
				v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
						"The NodeHandler can only be initialised once."));
	}
}

NodeHandler::NodeHandler(v8::Local<v8::Object> exports) {
	this->exports = exports;
}

void NodeHandler::exposeROOT() {
	exposeGlobals();
}

void NodeHandler::exposeGlobals() {
	TCollection *globals = gROOT->GetListOfGlobals();

	TIter next(globals);
	while (TObject *global = next()) {
		/*
		 * As we iterate through TObjects all these items can be pumped through
		 * the ObjectProxyFactory
		 */
		ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(
				*((TGlobal*) global));
		if (proxy != nullptr) {
			v8::Local<v8::String> name = v8::String::NewFromUtf8(
					v8::Isolate::GetCurrent(), global->GetName());

			CallbackHandler::setGlobalProxy(std::string(global->GetName()),
					proxy);

			this->exports->Set(name, proxy->get());
			this->exports->SetAccessor(name,
					&CallbackHandler::globalGetterCallback,
					&CallbackHandler::globalSetterCallback);
		}
	}
}

std::queue<std::string> NodeHandler::splitClassName(std::string name) {

	std::string delimiter = "::";
	std::queue<std::string> que;
	size_t pos = 0;
	std::string token;
	while ((pos = name.find(delimiter)) != std::string::npos) {
		token = name.substr(0, pos);
		que.push(token);
		name.erase(0, pos + delimiter.length());
	}
	que.push(name);

	return que;
}

}
