#include "NodeHandler.h"
#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"

#include <TROOT.h>
#include <iostream>

namespace RootJS {

	NodeHandler *NodeHandler::instance;
	bool NodeHandler::initialized;

	void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
		if(!initialized) {
			ObjectProxyFactory::initializeProxyMap();
			instance = new NodeHandler(exports);
			instance->exposeROOT();
		} else {
			v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "The NodeHandler can only be initialized once."));
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
		while(TObject *global = next()) {
			/*
			 * As we iterate through TObjects all these items can be pumped through
			 * the ObjectProxyFactory
			 * TODO: Implement something for scalar globals (often constants)
			 */
			ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(*((TGlobal*)global));
			if(proxy != nullptr) {
				this->exports->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), global->GetName()), proxy->get());
			}
		}
	}
}
