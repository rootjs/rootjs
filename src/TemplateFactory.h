#ifndef TEMPLATEFACTORY_H_
#define TEMPLATEFACTORY_H_

#include "CallbackHandler.h"

#include <map>
#include <string>
#include <stdexcept>

#include <TClass.h>
#include <TEnum.h>

class CallbackHandler;

namespace rootJS
{
	class TemplateFactory
	{
		public:
			virtual ~TemplateFactory();

			/**
			 * Finds out the type of the given TClass and calls the correct member function
			 * to create a v8 template of it, then returns the template.
			 *
			 * @param *clazz Pointer to the TClass to be converted to a v8 template.
			 *
			 * @return The generated v8 template
			 */
			static v8::Local<v8::Object> getInstance(TClass *clazz) throw(std::invalid_argument);

			/**
			 * Finds out the type of the given TClass and calls the correct member function
			 * to create a v8 constructor of it, then returns the template.
			 *
			 * @param *clazz Pointer to the TClass
			 *
			 * @return The v8 constructor for the given TClass
			 */
			static v8::Local<v8::Function> getConstructor(TClass *clazz) throw(std::invalid_argument);

		private:
			static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> classTemplates;
			static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> structTemplates;

			static void createInstantiableTemplate(TClass *clazz, v8::Local<v8::FunctionTemplate> tmplt) throw(std::invalid_argument);
			static void addEnumTemplate(TClass *clazz, v8::Local<v8::ObjectTemplate> tmplt) throw(std::invalid_argument);

			static inline bool isTemplateFunction(std::string const& functionName);
			static bool isValid(TClass *clazz);

			static v8::Local<v8::ObjectTemplate> createNamespaceTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::Object> initializeNamespace(TClass *clazz) throw(std::invalid_argument);

			static v8::Local<v8::Object> initializeEnum(TEnum *eNum) throw(std::invalid_argument);
			static v8::Local<v8::ObjectTemplate> createEnumTemplate(TEnum *eNum) throw(std::invalid_argument);

			static v8::Local<v8::ObjectTemplate> createArrayTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::FunctionTemplate> createUnionTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::FunctionTemplate> createStructTemplate(TClass *clazz) throw(std::invalid_argument);
			static v8::Local<v8::FunctionTemplate> createClassTemplate(TClass *clazz) throw(std::invalid_argument);

			TemplateFactory();
	};
}

#endif
