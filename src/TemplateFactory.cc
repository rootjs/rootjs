#include "TemplateFactory.h"

namespace RootJS
{
	// Initialize static class members
	std::map<std::string, v8::Persistent<v8::FunctionTemplate>> TemplateFactory::templates;

	TemplateFactory::TemplateFactory()
	{}

	TemplateFactory::~TemplateFactory()
	{}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createTemplate(TClassRef const& classRef)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();

		TClass *clazz = classRef.GetClass();
		std::string name(clazz->GetName());

		// Check if template has been already created
		if (templates.count(name) && !templates[name].IsEmpty())
		{
			// std::cout << "using cache to create: " << name << std::endl;
			return v8::Local<v8::FunctionTemplate>::New(isolate, templates[name]);
		}

		v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, CallbackHandler::ctorCallback); // v8::External::New(isolate, classRef)
		prototype->SetClassName(v8::String::NewFromUtf8(isolate, name.c_str()));

		v8::Local<v8::ObjectTemplate> instance = prototype->InstanceTemplate();
		instance->SetInternalFieldCount(1); // each instance stores a reference to an ObjectProxy

		// instance->SetNamedPropertyHandler(getGetterCallback(), getSetterCallback());
		// instance->Set(isolate, "data", v8::Number::New(isolate, 5));
		// instance->SetAccessor(String::NewFromUtf8(isolate, "data"), getGetterCallback(), getSetterCallback());

		/*
		 * TODO: add members and functions
		 */

		templates[name].Reset(isolate, prototype);
		v8::Local<v8::FunctionTemplate> tmp = v8::Local<v8::FunctionTemplate>::New(isolate, templates[name]);

		return tmp;
	}
}
