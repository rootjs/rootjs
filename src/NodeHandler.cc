#include "NodeHandler.h"
#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "CallbackHandler.h"
#include "NodeApplication.h"
#include "TemplateFactory.h"

#include <TROOT.h>
#include <string>

namespace rootJS {

NodeHandler *NodeHandler::instance;
bool NodeHandler::initialized;

void NodeHandler::exposeGlobalFunctions() {
}

void NodeHandler::exposeMacros() {
}

void NodeHandler::exposeClasses() {
	TCollection* classes = gROOT->GetListOfClasses();
	TIter next(classes);
	while (TObject *clazz = next()) {
		if (((TClass*) clazz)->Property() & kIsClass) {
			std::stack<std::string> stk;
			//TODO split class name by "::" and put it on a stack
			this->exports->Set(
					v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
							TClassRef((TClass*) clazz)->GetName()),
					exposeClassRec(stk.top(), stk)->GetFunction());
		}
	}
}

v8::Local<v8::FunctionTemplate> NodeHandler::exposeClassRec(std::string name,
		std::stack<std::string>& stk) {
	std::string curname = name;
	stk.pop();
	DictFuncPtr_t dictFunc = gClassTable->GetDict(name.c_str());
	//TODO implement map to avoid duplicates and check for double assignments of identical
	// classes to the same class/name space and avoid them
	if (dictFunc == nullptr) {
		//TODO throw exception
	}
	if (stk.empty()) {
		return TemplateFactory::createTemplate(TClassRef(dictFunc()));
	} else {
		v8::Local<v8::FunctionTemplate> rectmpl = exposeClassRec(stk.top(), stk);
		v8::Local<v8::FunctionTemplate> curtmpl = TemplateFactory::createTemplate(TClassRef(dictFunc()));
		curtmpl->Set(v8::Isolate::GetCurrent(),curname.c_str(),rectmpl);
		return curtmpl;
	}
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

}
