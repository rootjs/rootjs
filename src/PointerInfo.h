#ifndef POINTER_INFO_H_
#define POINTER_INFO_H_

#include "MetaInfo.h"

namespace rootJS
{

	/**
	 * This class contains the info for a pointer
	 */
	class PointerInfo: public MetaInfo
	{
		public:
			PointerInfo(void* baseAddr, const char* typeName, int ptrDepth = 1);
			~PointerInfo();

			virtual bool isGlobal()
			{
				return true;
			};

			virtual Long_t GetOffset()
			{
				return 0;
			};

			virtual bool isConst()
			{
				return true;
			};

			virtual bool isStatic()
			{
				return true;
			};

			virtual const char* getTypeName();
			virtual const char* getFullTypeName();
			virtual const char* getName() {
				return "anonymous";
			};

			virtual PointerInfo* clone()
			{
				return new PointerInfo(baseAddress, typeName, ptrDepth);
			};

		protected:
			/**
			 * Type of the pointer
			 */
			char *typeName;
	};
}

#endif
