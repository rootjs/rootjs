#ifndef MEMBERINFO_H_
#define MEMBERINFO_H_

#include "MetaInfo.h"

#include <TDataMember.h>

namespace rootJS
{

	/**
	 * This class contains the info for a TDataMember
	 */
	class MemberInfo : public MetaInfo
	{
		public:
			MemberInfo(const TDataMember &, void* baseAddress);
			~MemberInfo();

			virtual bool isGlobal();
			virtual Long_t getOffset();

			virtual bool isConst();
			virtual bool isStatic();

			virtual const char* getTypeName();
			virtual const char* getName();

			/** The type the MemberInfo is holding.  */
			const TDataMember &currentObject;

			virtual MetaInfo* clone()
			{
				return new MemberInfo(currentObject, baseAddress);
			};
	};
}
#endif
