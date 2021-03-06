#include "MemberInfo.h"

namespace rootJS
{

	MemberInfo::MemberInfo(const TDataMember& type, void* baseAddress, int ptrDepth):
	MetaInfo(baseAddress, ptrDepth),
	type(type)
	{}

	MemberInfo::~MemberInfo()
	{}

	Long_t MemberInfo::getOffset()
	{
		return type.GetOffset();
	}

	bool MemberInfo::isGlobal()
	{
		return false;
	}

	bool MemberInfo::isConst()
	{
		return (type.Property() & (kIsConstant | kIsEnum));
	}

	bool MemberInfo::isStatic()
	{
		return (type.Property() & kIsStatic);
	};

	const char* MemberInfo::getTypeName()
	{
		return type.GetTypeName();
	}

	const char* MemberInfo::getFullTypeName()
	{
		return type.GetFullTypeName();
	}

	const char* MemberInfo::getName()
	{
		return type.GetName();
	}

	MemberInfo* MemberInfo::clone()
	{
		return new MemberInfo(type, baseAddress, ptrDepth);
	}
}
