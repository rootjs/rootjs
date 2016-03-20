#include "NodeHandler.h"

#include "NodeApplication.h"
#include "GlobalInfo.h"
#include "EnumInfo.h"
#include "Toolbox.h"
#include "Types.h"

#include <queue>

#include <TROOT.h>
#include <TEnum.h>
#include <TClassTable.h>
#include <TSystem.h>

namespace rootJS
{

	NodeHandler *NodeHandler::instance;
	bool NodeHandler::initialized;

	void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
	{
		if (!initialized)
		{

			NodeApplication::CreateNodeApplication();
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
		this->exportPersistent.Reset(exports);
		this->exports = exports;
		initialized = true;
	}


	NodeHandler* NodeHandler::getInstance()
	{
		if(initialized)
		{
			return NodeHandler::instance;
		}
		else
		{
			throw std::invalid_argument("NodeHandler not loaded yet");
		}
	}

	void NodeHandler::exposeROOT()
	{
		gInterpreter->SetClassAutoloading(kTRUE);

		try
		{
			exposeGlobals();
			exposeGlobalEnums();
			exposeGlobalFunctions();
			exposeClasses();
			exposeInterfaceFunctions();
		}
		catch (const std::invalid_argument& e)
		{
			Toolbox::throwException(e.what());
			return;
		}
	}

	void NodeHandler::exposeGlobals() throw (std::invalid_argument)
	{
		TCollection *globals = gROOT->GetListOfGlobals(kTRUE);
		TIter next(globals);
		v8::Local<v8::Object> exportsLocal = Nan::New(exportPersistent);
		while (TGlobal *global = (TGlobal*) next())
		{
			if( (!global->IsValid()) || (global->GetAddress() == nullptr))
			{
				Toolbox::logInfo("Invalid global instance found.",1);
				continue;
			}

			if(exportsLocal->Has(Nan::New(global->GetName()).ToLocalChecked()))
			{
				continue;
			}

			std::string fullTypeName(global->GetFullTypeName());
			int derefCount = std::count(fullTypeName.begin(), fullTypeName.end(), '*');

			GlobalInfo info(*global, derefCount + 2);
			ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, nullptr);
			if (proxy != nullptr)
			{
				CallbackHandler::registerGlobalObject(std::string(global->GetName()), proxy);
				Nan::SetAccessor(exportsLocal, Nan::New(global->GetName()).ToLocalChecked(),
											CallbackHandler::globalGetterCallback,
											CallbackHandler::globalSetterCallback);
			}
		}
	}

	void NodeHandler::exposeGlobalEnums() throw (std::invalid_argument)
	{
		TCollection *enums = gROOT->GetListOfEnums(kTRUE);
		TIter next(enums);
		v8::Local<v8::Object> exportsLocal = Nan::New(exportPersistent);

		while (TEnum *eNum = (TEnum*) next())
		{
			// skip non global enums
			if(eNum->GetClass() != nullptr)
			{
				continue;
			}

			if(!eNum->IsValid())
			{
				Toolbox::logInfo("Invalid enum found.", 1);
				continue;
			}

			if(exportsLocal->Has(Nan::New(eNum->GetName()).ToLocalChecked()))
			{
				continue;
			}

			void* internalField = Nan::GetInternalFieldPointer(TemplateFactory::encapsulateEnum(eNum), Toolbox::ObjectProxyPtr);
			CallbackHandler::registerGlobalObject(eNum->GetName(), (ObjectProxy*) internalField);
			Nan::SetAccessor(exportsLocal, Nan::New(eNum->GetName()).ToLocalChecked(),
										CallbackHandler::globalGetterCallback,
										CallbackHandler::globalSetterCallback);
		}
	}

	void NodeHandler::exposeGlobalFunctions() throw (std::invalid_argument)
	{

		TCollection *functions = gROOT->GetListOfGlobalFunctions(kTRUE);

		v8::Local<v8::Object> exportsLocal = Nan::New(exportPersistent);

		TIter next(functions);
		while (TFunction *function = (TFunction*) next())
		{
			if (!function->IsValid())
			{
				Toolbox::logInfo("Invalid global function found.",1);
				continue;
			}

			if(exportsLocal->Has(Nan::New(function->GetName()).ToLocalChecked()))
			{
				continue;
			}

			std::string exposeName(function->GetName());
			if(function->ExtraProperty() & kIsOperator)
			{
				std::map<std::string, std::string>::const_iterator opNameIt = Types::operatorNames.find(function->GetName());
				if(opNameIt != Types::operatorNames.end())
				{
					exposeName = opNameIt->second;
				}
			}

			v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(function->GetName(), nullptr);
			exportsLocal->Set(Nan::New(exposeName.c_str()).ToLocalChecked(),
			                  Nan::New<v8::Function>(CallbackHandler::globalFunctionCallback, data));
		}
	}

	void NodeHandler::exposeClasses() throw (std::invalid_argument)
	{
		//TODO implement maps for dynamic loading
		v8::Local<v8::Object> exportsLocal = Nan::New(exportPersistent);
		for (int i = 0; i < gClassTable->Classes(); i++)
		{
			DictFuncPtr_t funcPtr = gClassTable->GetDict(gClassTable->At(i));
			if (funcPtr == nullptr)
			{
				throw std::invalid_argument(
				    std::string("Specified class is null."));
			}

			TClass *clazz = funcPtr(); // call dictionary function on class
			if (clazz == nullptr || !clazz->IsLoaded())
			{
				throw std::invalid_argument(
				    std::string("Specified class is not loaded."));
			}
			if ((((std::string) clazz->GetName()).find(":") == std::string::npos) && (!exportsLocal->Has(Nan::New(clazz->GetName()).ToLocalChecked())))
			{
				if ((clazz->Property() & kIsClass))
				{
					Toolbox::logInfo(std::string("loading class ").append(clazz->GetName()), 2);
					exportsLocal->Set(Nan::New(clazz->GetName()).ToLocalChecked(), TemplateFactory::getConstructor(clazz));
					continue;
				}
				if((clazz->Property() & kIsNamespace))
				{
					Toolbox::logInfo(std::string("loading namespace ").append(clazz->GetName()), 2);
					exportsLocal->Set(Nan::New(clazz->GetName()).ToLocalChecked(),TemplateFactory::getInstance(clazz));
					continue;
				}
			}
			else
			{
				exposeHierarchy(clazz, exportsLocal);
			}
		}
	}

	void NodeHandler::exposeHierarchy(TClass* clazz,v8::Local<v8::Object> exports)  throw(std::invalid_argument)
	{
		std::vector<std::string> vec;
		std::string name(clazz->GetName());
		if(name.find('<') != std::string::npos)
		{
			//TODO Handle templates
			Toolbox::logInfo(std::string("omitting template ").append(clazz->GetName()),2);
			return;
		}
		splitClassName(name, vec);
		std::queue<std::string> pathque;
		std::queue<std::string> nameque;
		std::string buff = vec[0];
		nameque.push(buff);
		pathque.push(buff);

		//setting up the names under which the object are exported and their names in ROOT
		for(uint i = 1; i < vec.size(); i++)
		{
			std::string buff = vec[i];
			nameque.push(buff);
			std::string pathbuff = pathque.back();
			pathbuff.append("::");
			pathbuff.append(buff);
			pathque.push(pathbuff);
		}

		v8::Local<v8::Object>& scope = exports;
		while(!nameque.empty())
		{
			v8::Local<v8::Object> obj;
			if (!scope->Has(Nan::New(nameque.front().c_str()).ToLocalChecked()))
			{
				DictFuncPtr_t funcPtr(gClassTable->GetDict(pathque.front().c_str()));
				if (funcPtr == nullptr)
				{
					Toolbox::logInfo(std::string("creating stub namespace: ").append(pathque.front()), 2);
					scope->Set(Nan::New(nameque.front().c_str()).ToLocalChecked(), Nan::New<v8::Object>());
				}
				else
				{
					try
					{
						TClass *curclazz = funcPtr();
						if (curclazz->Property() & kIsNamespace)
						{
							Toolbox::logInfo(std::string("loading namespace ").append(curclazz->GetName()),2);
							obj = TemplateFactory::getInstance(curclazz);
							scope->Set(Nan::New(nameque.front().c_str()).ToLocalChecked(), obj);
						}
						if (curclazz->Property() & kIsClass)
						{
							Toolbox::logInfo(std::string("loading class ").append(curclazz->GetName()),2);
							obj = TemplateFactory::getConstructor(curclazz);
							scope->Set(Nan::New(nameque.front().c_str()).ToLocalChecked(), obj);
						}
					}
					catch (const std::invalid_argument &e)
					{
						throw std::invalid_argument(e);
					}
				}
				//TODO figure out if there are other cases as well
			}

			scope = v8::Local<v8::Object>::Cast(scope->Get(Nan::New(nameque.front().c_str()).ToLocalChecked()));
			pathque.pop();
			nameque.pop();
		}

	}

	void NodeHandler::loadlibraryCallback(const Nan::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument)
	{
		v8::Local<v8::Value> arg;
		if(!((info.Length() == 1) && ((arg = info[0])->IsString())))
		{
			throw std::invalid_argument("Usage: Pass name of the library");
		}
		std::string libname = Toolbox::Stringv8toStd(v8::Local<v8::String>::Cast(arg));
		int rcode = gSystem->Load(libname.c_str());
		info.GetReturnValue().Set(rcode);
		if(rcode >= 0)
		{
			NodeHandler::getInstance()->refreshExports();
		}
	}

	void NodeHandler::refreshExportsCallback(const Nan::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument)
	{
		if(info.Length() != 0)
		{
			throw std::invalid_argument("Usage: No arguments. Refreshes the exported functions");
		}
		NodeHandler::getInstance()->refreshExports();
	}

	void NodeHandler::refreshExports()
	{
		exposeGlobals();
		exposeGlobalFunctions();
		exposeClasses();
	}


	void NodeHandler::exposeInterfaceFunctions()
	{
		v8::Local<v8::Object> exportsLocal = Nan::New(exportPersistent);
		exportsLocal->Set(Nan::New("loadlibrary").ToLocalChecked(),
		                  Nan::New<v8::Function>(NodeHandler::loadlibraryCallback));
		exportsLocal->Set(Nan::New("refreshExports").ToLocalChecked(),Nan::New<v8::Function>(NodeHandler::refreshExportsCallback));
	}


	std::vector<std::string> NodeHandler::splitClassName(std::string name, std::vector<std::string>& vec)
	{

		std::string delimiter = "::";
		size_t pos = 0;
		std::string token;
		while ((pos = name.find(delimiter)) != std::string::npos)
		{
			token = name.substr(0, pos);
			vec.push_back(token);
			name.erase(0, pos + delimiter.length());
		}
		vec.push_back(name);

		return vec;
	}


}
