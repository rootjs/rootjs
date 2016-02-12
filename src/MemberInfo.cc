#include "MemberInfo.h"
namespace rootJS
{

	MemberInfo::MemberInfo(const TDataMember& type, void* baseAddress): MetaInfo(baseAddress), currentObject(type)
	{
	}

	MemberInfo::~MemberInfo()
	{}

	bool MemberInfo::isGlobal()
	{
		return false;
	}

	Long_t MemberInfo::getOffset()
	{
		return currentObject.GetOffset();
	}

	bool MemberInfo::isConst()
	{
		if(currentObject.Property() & kIsConstant)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool MemberInfo::isStatic()
	{
		if(currentObject.Property() & kIsStatic)
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	const char* MemberInfo::getTypeName()
	{
		return currentObject.GetTypeName();
	}

	const char* MemberInfo::getName()
	{
		return currentObject.GetName();
	}
}
