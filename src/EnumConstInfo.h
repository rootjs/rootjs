#ifndef ENUMCONSTINFO_H_
#define ENUMCONSTINFO_H_

#include "MetaInfo.h"

#include <TEnumConstant.h>

namespace rootJS
{

	class EnumConstInfo : public MetaInfo
	{
		public:

			EnumConstInfo(const TEnumConstant & type);
			~EnumConstInfo();

			virtual Long_t GetOffset();

			virtual bool isGlobal();
			virtual bool isConst();
			virtual bool isStatic();

			virtual const char* getTypeName();
			virtual const char* getName();

			virtual EnumConstInfo* clone();

		private:
			/** The type the EnumConstInfo is holding.  */
			const TEnumConstant &type;
	};

}

#endif
