#ifndef MEMBERINFO_H_
#define MEMBERINFO_H_

#include "TDataMember.h"
#include "MetaInfo.h"

namespace rootJS
{

	class MemberInfo : public MetaInfo
	{
		public:
			MemberInfo(TDataMember *member);
			virtual ~MemberInfo();

			virtual TDataMember* getInfo();

			virtual Long_t getOffset();
			virtual Long_t getProperty();

			virtual std::string getName();
			virtual std::string getTypeName();
			virtual std::string getFullTypeName();

			TClass* getClass();

		private:
			TDataMember *member;
	};

}

#endif
