#include "GlobalInfo.h"

#include "Rtypes.h"
#include "TClass.h"
#include "TClassTable.h"

namespace rootJS
{

	GlobalInfo::GlobalInfo(TGlobal *global)
	{
		if(global == nullptr || !global->IsValid())
		{
			// TODO
		}
		this->global = global;
	}

	GlobalInfo::~GlobalInfo()
	{}

	TGlobal* GlobalInfo::getInfo()
	{
		return global;
	}

	Long_t GlobalInfo::getOffset()
	{
		return 0;
	}

	Long_t GlobalInfo::getProperty()
	{
		return global->Property();
	}

	std::string GlobalInfo::getName()
	{
		return std::string(global->GetName());
	}

	std::string GlobalInfo::getTypeName()
	{
		return std::string(global->GetTypeName());
	}

	std::string GlobalInfo::getFullTypeName()
	{
		return std::string(global->GetFullTypeName());
	}

	TClass* GlobalInfo::getClass()
	{
		/* if(!(getProperty() & kIsClass)) return nullptr; */

		DictFuncPtr_t classPtr = gClassTable->GetDict(global->GetTypeName());
		TClass *clazz = nullptr;

		if(classPtr == nullptr || (clazz = classPtr()) == nullptr /* || !clazz->IsLoaded() */)
		{
			return nullptr;
		}

		return clazz;
	}
}
