#ifndef TEMPLATEFACTORY_H_
#define TEMPLATEFACTORY_H_

#include "CallbackHandler.h"

#include <map>
#include <string>
#include <stdexcept>

#include <TClass.h>

class CallbackHandler;

namespace rootJS
{
	class TemplateFactory
	{
		public:
			virtual ~TemplateFactory();

			static v8::Local<v8::Object> getInstance(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::Function> getConstructor(TClass *clazz) throw(std::invalid_argument);

			static v8::Local<v8::ObjectTemplate> createNamespaceTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::Object> initializeNamespace(TClass *clazz) throw(std::invalid_argument);

			static v8::Local<v8::ObjectTemplate> createEnumTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::ObjectTemplate> createArrayTemplate(TClass *clazz) throw(std::invalid_argument);

			static v8::Local<v8::FunctionTemplate> createUnionTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::FunctionTemplate> createStructTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::FunctionTemplate> createClassTemplate(TClass *clazz) throw(std::invalid_argument);

		private:
			static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> classTemplates;
			static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> structTemplates;

			static void createInstantiableTemplate(TClass *clazz, v8::Local<v8::FunctionTemplate> tmplt) throw(std::invalid_argument);

			static inline bool isTemplateFunction(std::string const& functionName);
			static bool isValid(TClass *clazz);

			TemplateFactory();
	};
}

#endif
