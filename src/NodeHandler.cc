#include "NodeHandler.h"

#include "NodeApplication.h"
#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "CallbackHandler.h"
#include "Toolbox.h"

#include <string>

#include <TROOT.h>
#include <TClass.h>
#include <Rtypes.h>
#include <TClassTable.h>
#include <TInterpreter.h>

namespace rootJS
{

	NodeHandler *NodeHandler::instance;
	bool NodeHandler::initialized;

	void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
	{
		// assert(sizeof(Long_t) == sizeof(void*));

		if(!initialized)
		{
			NodeApplication::CreateNodeApplication();
			ObjectProxyFactory::initializeProxyMap();
			instance = new NodeHandler(exports);
			instance->exposeROOT();
		}
		else
		{
			Toolbox::throwException("The NodeHandler can only be initialized once.");
		}
	}

	NodeHandler::NodeHandler(v8::Local<v8::Object> exports)
	{
		this->exports = exports;
	}

	void NodeHandler::exposeROOT()
	{
		exposeGlobals();
		exposeGlobalFunctions();
	}

	void NodeHandler::exposeGlobals()
	{
		TCollection *globals = gROOT->GetListOfGlobals(kTRUE);

		TIter next(globals);
		while(TObject *global = next())
		{
			/*
			 * As we iterate through TObjects all these items can be pumped through
			 * the ObjectProxyFactory
			 * TODO: Implement something for scalar globals (often constants)
			 */
			ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(*((TGlobal*)global));
			if(proxy != nullptr)
			{
				v8::Local<v8::String> name = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), global->GetName());

				CallbackHandler::registerGlobalObject(std::string(global->GetName()), proxy);
				// this->exports->Set(name, proxy->get());
				this->exports->SetAccessor(name, CallbackHandler::globalGetterCallback, CallbackHandler::globalSetterCallback);
			}
		}
	}

	void NodeHandler::exposeGlobalFunctions()
	{
		TCollection *globals = gROOT->GetListOfGlobalFunctions(kTRUE);

		TIter next(globals);
		while(TObject *global = next())
		{
			v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(global->GetName(), nullptr);
			exports->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), global->GetName()), v8::Function::New(v8::Isolate::GetCurrent(), CallbackHandler::globalFunctionCallback, data));
		}
	}

	void NodeHandler::exposeClasses() throw(std::invalid_argument)
	{
		gInterpreter->SetClassAutoloading(kTRUE); // maybe not necessary

		for (int i = 0; i < gClassTable->Classes(); i++)
		{


			DictFuncPtr_t funcPtr = gClassTable->GetDict(gClassTable->At(i));
			if (funcPtr == nullptr)
			{
				throw std::invalid_argument(std::string("Specified class is null."));
			}

			TClass *clazz = funcPtr(); // call dictionary function on class
			if (clazz == nullptr || !clazz->IsLoaded())
			{
				throw std::invalid_argument(std::string("Specified class is not loaded."));
			}
			if (((std::string) clazz->GetName()).find(":") == std::string::npos)
			{

				if (clazz->Property() & kIsNamespace)
				{
					this->exports->Set(v8::String::NewFromUtf8(
					                       v8::Isolate::GetCurrent(), clazz->GetName()),TemplateFactory::getInstance(clazz));
				}

			}
			else if (clazz->Property() & kIsClass)
			{
				this->exports->Set(v8::String::NewFromUtf8(
				                       v8::Isolate::GetCurrent(), clazz->GetName()),TemplateFactory::getConstructor(clazz));
			}
		}
	}

}



