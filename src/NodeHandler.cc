#include "NodeHandler.h"

#include "Proxy.h"
#include "GlobalInfo.h"

#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "CallbackHandler.h"
#include "NodeApplication.h"

#include <TROOT.h>
#include <string>

namespace rootJS
{

	NodeHandler *NodeHandler::instance;
	bool NodeHandler::initialized;

	void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
	{
		if(!initialized)
		{
			NodeApplication::CreateNodeApplication();
			ObjectProxyFactory::initializeProxyMap();
			instance = new NodeHandler(exports);
			instance->exposeROOT();
		}
		else
		{
			v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "The NodeHandler can only be initialized once."));
		}
	}

	NodeHandler::NodeHandler(v8::Local<v8::Object> exports)
	{
		this->exports = exports;
	}

	void NodeHandler::exposeROOT()
	{
		exposeGlobals();
	}

	void NodeHandler::exposeGlobals()
	{
		TCollection *globals = gROOT->GetListOfGlobals();

		TIter next(globals);
		while(TObject *global = next())
		{

			if( !((TGlobal*)global)->IsValid() ) {
				continue;
			}

			/*
			 * As we iterate through TObjects all these items can be pumped through
			 * the ObjectProxyFactory
			 * TODO: Implement something for scalar globals (often constants)
			 */
			void *address = ((TGlobal*)global)->GetAddress();
			GlobalInfo *type = new GlobalInfo(((TGlobal*) global));

			ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(address, type, nullptr);
			if(proxy != nullptr)
			{
				v8::Local<v8::String> name = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), global->GetName());

				CallbackHandler::registerGlobalObject(std::string(global->GetName()), proxy);

				this->exports->Set(name, proxy->get());
				this->exports->SetAccessor(
				    name,
				    &CallbackHandler::globalGetterCallback,
				    &CallbackHandler::globalSetterCallback
				);
			}
		}
	}
}
