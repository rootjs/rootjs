#include "MemberInfo.h"

namespace rootJS
{

	MemberInfo::MemberInfo(const TDataMember& type, void* baseAddress): MetaInfo(baseAddress), type(type)
	{}

	MemberInfo::~MemberInfo()
	{}

	Long_t MemberInfo::getOffset()
	{
		return type.GetOffset();
	}

	bool MemberInfo::isArray()
	{
		return (type.Property() & kIsArray);
	}

	bool MemberInfo::isGlobal()
	{
		return false;
	}

	bool MemberInfo::isConst()
	{
		return (type.Property() & kIsConstant);
	}

	bool MemberInfo::isStatic()
	{
		return (type.Property() & kIsStatic);
	};

	int MemberInfo::getArrayLength()
	{
		// TODO see GlobalInfo::getArrayLength
		return type.GetMaxIndex(0);
	}

	const char* MemberInfo::getTypeName()
	{
		return type.GetTypeName();
	}

	const char* MemberInfo::getName()
	{
		return type.GetName();
	}

	MemberInfo* MemberInfo::clone()
	{
		return new MemberInfo(type, baseAddress);
	}
}
