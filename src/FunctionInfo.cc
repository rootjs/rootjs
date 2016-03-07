#include "FunctionInfo.h"

namespace rootJS
{

	FunctionInfo::FunctionInfo(const TFunction& type, void* baseAddress) : FunctionInfo(type, baseAddress, false)
	{}

	FunctionInfo::FunctionInfo(const TFunction& type, void* baseAddress, bool isGlobal) : MetaInfo(baseAddress, 2), type(type), mIsGlobal(isGlobal)
	{}

	FunctionInfo::FunctionInfo(const FunctionInfo & info) : MetaInfo(info.baseAddress), type(info.type), mIsGlobal(info.mIsGlobal)
	{}

	FunctionInfo::~FunctionInfo()
	{}

	Long_t FunctionInfo::getOffset()
	{
		return 0;
	}

	bool FunctionInfo::isGlobal()
	{
		return mIsGlobal;
	}

	bool FunctionInfo::isConst()
	{
		return (type.Property() & kIsConstant);
	}

	bool FunctionInfo::isStatic()
	{
		return (type.Property() & kIsStatic);
	};

	const char* FunctionInfo::getTypeName()
	{
		return type.GetReturnTypeName();
	}

	const char* FunctionInfo::getFullTypeName()
	{
		return type.GetReturnTypeName();
	}

	const char* FunctionInfo::getName()
	{
		return type.GetName();
	}

	FunctionInfo* FunctionInfo::clone()
	{
		return new FunctionInfo(type, baseAddress, mIsGlobal);
	}
}
