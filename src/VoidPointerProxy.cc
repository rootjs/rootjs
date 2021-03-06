#include "VoidPointerProxy.h"

#include "Toolbox.h"

#include <string>
#include <sstream>

namespace rootJS
{

	VoidPointerProxy::VoidPointerProxy(MetaInfo &info, TClass *scope) : PrimitiveProxy(info, scope)
	{}

	VoidPointerProxy::~VoidPointerProxy()
	{}

	v8::Local<v8::Value> VoidPointerProxy::get()
	{
		void *p = getAddress();
		if(p != nullptr) {
			std::ostringstream stm;
			stm << ((Long_t) p);
			return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), stm.str().c_str());
		} else {
			return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "nullptr");
		}
	}

	void VoidPointerProxy::setValue(v8::Local<v8::Value> value)
	{
		Toolbox::throwException("Setting void pointers is not supported yet.");
	}

	ObjectProxy* VoidPointerProxy::voidConstruct(MetaInfo &info, TClass *scope)
	{
		return new VoidPointerProxy(info, scope);
	}

	bool VoidPointerProxy::isConst()
	{
		return true;
	}

}
