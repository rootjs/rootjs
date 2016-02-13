#ifndef FUNCTION_INFO_H_
#define FUNCTION_INFO_H_

#include "MetaInfo.h"

#include <TFunction.h>

namespace rootJS
{

	/**
	 * This class contains the info for a TFunction
	 */
	class FunctionInfo: public MetaInfo
	{
		public:
			FunctionInfo(const TFunction& type, void* baseAddress);
			FunctionInfo(const TFunction& type, void* baseAddress, bool isGlobal);
			~FunctionInfo();

			virtual Long_t getOffset();

			virtual bool isGlobal();
			virtual bool isConst();
			virtual bool isStatic();

			virtual const char* getTypeName();
			virtual const char* getName();

			virtual FunctionInfo* clone();

		private:
			/**
			 * The function the FunctionInfo is holding.
			 */
			const TFunction &type;
			const bool mIsGlobal;
	};
}

#endif
