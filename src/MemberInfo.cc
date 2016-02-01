#include "MemberInfo.h"

#include "TClass.h"
#include "TClassTable.h"
#include "Rtypes.h"

namespace rootJS
{

	MemberInfo::MemberInfo(TDataMember *member) : member(member)
	{}

	MemberInfo::~MemberInfo()
	{}

	TDataMember* MemberInfo::getInfo()
	{
		return member;
	}

	Long_t MemberInfo::getOffset()
	{
		if(getProperty() & kIsStatic)
		{
			return 0;
			// return member->GetOffsetCint();
		}
		else
		{
			// return member->GetOffset();
			return member->GetOffsetCint();
		}
	}

	Long_t MemberInfo::getProperty()
	{
		return member->Property();
	}

	std::string MemberInfo::getName()
	{
		return std::string(member->GetName());
	}

	std::string MemberInfo::getTypeName()
	{
		return std::string(member->GetTypeName());
	}

	std::string MemberInfo::getFullTypeName()
	{
		return std::string(member->GetFullTypeName());
	}

	TClass* MemberInfo::getClass()
	{
		/* if(!(getProperty() & kIsClass)) return nullptr; */

		DictFuncPtr_t classPtr = gClassTable->GetDict(member->GetTypeName());
		TClass *clazz = nullptr;

		if(classPtr == nullptr || (clazz = classPtr()) == nullptr /* || !clazz->IsLoaded() */)
		{
			return nullptr;
		}

		return clazz;
	}

}
