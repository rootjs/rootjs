#ifndef ENUMINFO_H_
#define ENUMINFO_H_

#include "MetaInfo.h"

#include <TEnum.h>

namespace rootJS
{

	class EnumInfo : public MetaInfo
	{
		public:
			EnumInfo(const TEnum & type);
			~EnumInfo();

			virtual Long_t GetOffset();

			virtual bool isGlobal();
			virtual bool isConst();
			virtual bool isStatic();

			virtual const char* getTypeName();
			virtual const char* getName();

			virtual EnumInfo* clone();

		private:
			/** The type the EnumInfo is holding.  */
			const TEnum &type;
	};

}

#endif
