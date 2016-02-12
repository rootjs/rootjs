#include "NodeHandler.h"
#include <v8.h>
#include "NodeApplication.h"
#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "CallbackHandler.h"
#include "Toolbox.h"
#include "ClassExposer.h"
#include <string>
#include <iostream>
#include <TROOT.h>
#include <TInterpreter.h>
#include <TFunction.h>
#include <TGlobal.h>
#include <TClass.h>
#include <TClassTable.h>

namespace rootJS {

NodeHandler *NodeHandler::instance;
bool NodeHandler::initialized;

void NodeHandler::initialize(v8::Local<v8::Object> exports,
		v8::Local<v8::Object> module) {

	if (!initialized) {

		NodeApplication::CreateNodeApplication();
		ObjectProxyFactory::initializeProxyMap();
		instance = new NodeHandler(exports);
		instance->exposeROOT();

	} else {
		Toolbox::throwException(
				"The NodeHandler can only be initialized once.");
	}
}

NodeHandler::NodeHandler(v8::Local<v8::Object> exports) {
	this->exports = exports;
}

void NodeHandler::exposeROOT() {
	gInterpreter->SetClassAutoloading(kTRUE);

	try {
		exposeGlobals();
		exposeGlobalFunctions();
		exposeMacros();

		exposeClasses();
	} catch (const std::invalid_argument& e) {
		Toolbox::throwException(e.what());
		return;
	}
}

void NodeHandler::exposeMacros() throw (std::invalid_argument) {
	// TODO
}

void NodeHandler::exposeGlobals() throw (std::invalid_argument) {
	TCollection *globals = gROOT->GetListOfGlobals(kTRUE);
	TIter next(globals);

	while (TGlobal *global = (TGlobal*) next()) {
		ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(*global);
		if (proxy != nullptr) {
			CallbackHandler::registerGlobalObject(
					std::string(global->GetName()), proxy);
			this->exports->SetAccessor(
					v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
							global->GetName()),
					CallbackHandler::globalGetterCallback,
					CallbackHandler::globalSetterCallback);
		}
	}
}

void NodeHandler::exposeGlobalFunctions() throw (std::invalid_argument) {
	TCollection *functions = gROOT->GetListOfGlobalFunctions(kTRUE);

	TIter next(functions);
	while (TFunction *function = (TFunction*) next()) {
		if (!function->IsValid()) {
			Toolbox::logError("Invalid global function found.");
			continue;
		}

		v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(
				function->GetName(), nullptr);
		exports->Set(
				v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
						function->GetName()),
				v8::Function::New(v8::Isolate::GetCurrent(),
						CallbackHandler::globalFunctionCallback, data));
	}
}

void NodeHandler::exposeClasses() throw (std::invalid_argument) {
	for (int i = 0; i < gClassTable->Classes(); i++) {
		DictFuncPtr_t funcPtr = gClassTable->GetDict(gClassTable->At(i));
		if (funcPtr == nullptr) {
			throw std::invalid_argument(
					std::string("Specified class is null."));
		}

		TClass *clazz = funcPtr(); // call dictionary function on class
		if (clazz == nullptr || !clazz->IsLoaded()) {
			throw std::invalid_argument(
					std::string("Specified class is not loaded."));
		}
		const char* name = clazz->GetName();//TODO remove this line
		if (((std::string) clazz->GetName()).find(":") == std::string::npos) {
			if ((clazz->Property() & kIsClass)) {
				this->exports->Set(
					v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), clazz->GetName()), TemplateFactory::getConstructor(clazz));
				continue;
		}
			if((clazz->Property() & kIsNamespace)){
				this->exports->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),clazz->GetName()),TemplateFactory::getInstance(clazz));
				continue;
			}
		}
		else {
			ClassExposer::expose(clazz, exports);
		}
	}
}

}
