#ifndef TEMPLATEFACTORY_H_
#define TEMPLATEFACTORY_H_

#include <map>
#include <string>

#include "CallbackHandler.h"

#include "v8.h"

#include "TClassRef.h"
#include "TClass.h"

class CallbackHandler;

namespace RootJS
{
	class TemplateFactory
	{
	public:
		virtual ~TemplateFactory();

		/**
		 * Create a javascript prototype template of the specified ROOT class.
		 * Generated FunctionTemplate's may be used to create instances of javascript objects encapsulating objects of the specified ROOT class.
		 *
		 * @param clazz
		 *			the class to generate a template from
		 *
		 * @return the generated FunctionTemplate
		 */
		static v8::Local<v8::FunctionTemplate> createTemplate(TClassRef const& classRef);

		// static v8::Local<v8::FunctionTemplate> createTemplate(std::string const& name);

	private:
		static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> templates;

		TemplateFactory();
	};
}

#endif
