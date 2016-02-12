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
			FunctionInfo(TFunction* func) : MetaInfo(nullptr)
			{
				this->func = func;
			};

			~FunctionInfo()
			{}
			;

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

			virtual const char* getTypeName()
			{
				return func->GetReturnTypeName();
			};

			virtual const char* getName() {
				return func->GetName();
			}

			virtual MetaInfo* clone()
			{
				return new FunctionInfo(func);
			};

		protected:
			/**
			 * The function the FunctionInfo is holding.
			 */
			TFunction* func;
	};
}

#endif
