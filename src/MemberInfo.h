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

			virtual Long_t getOffset();

			virtual bool isArray();
			virtual bool isGlobal();
			virtual bool isConst();
			virtual bool isStatic();

			virtual int getArrayLength();
			virtual const char* getTypeName();
			virtual const char* getName();

			virtual MemberInfo* clone();

		private:
			/** The type the MemberInfo is holding.  */
			const TDataMember &type;
	};
}
#endif
