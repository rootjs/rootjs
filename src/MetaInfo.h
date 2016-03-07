#ifndef SRC_METAINFO_H_
#define SRC_METAINFO_H_

#include <RtypesCore.h>

#include "Toolbox.h"

#include <TString.h>

namespace rootJS
{

	/**
	 * This class encapsulates the differences in behaviour between TMember and
	 * TGlobal
	 * */
	class MetaInfo
	{
		protected:
			/**
			 * The base address of the specific TObject
			 */
			void* baseAddress;

			int ptrDepth;
			void *startAddress;
			void **ptr;
		public:
			/**
			 * Creates MetaInfo with a specific TObject and its base address
			 */
			MetaInfo(void *baseAddress, int ptrDepth): ptrDepth(ptrDepth)
			{
				this->baseAddress = baseAddress;
			};

			virtual ~MetaInfo() {};

			/**
			 * Checks if the TObject is global.
			 * @return If the TObject is global
			 */
			virtual bool isGlobal() = 0;

			/**
			 * Get the offset. This calls up
			 * the TDataMember::GetOffset() function.
			 * @return The offset
			 */
			virtual Long_t getOffset()
			{
				return 0;
			};

			/**
			 * Checks if the TObject is a constant.
			 * @return If the TObject is a constant
			 */
			virtual bool isConst() = 0;

			/**
			 * Checks if the TObject is static.
			 * @return If the TObject is static
			 */
			virtual bool isStatic() = 0;

			/**
			 * Returns the typename of the TObject.
			 * @return Typename of the TObject
			 */
			virtual const char* getTypeName() = 0;

			/**
			 * Returns the full typename of the TObject.
			 * @return full type name like char* instead of char (which is what getTypeNaem would return)
			 */
			virtual const char* getFullTypeName() = 0;

			/**
			 * Returns the name of the TObject.
			 * @return name of the TObject
			 */
			virtual const char* getName() = 0;

			/**
			 * Returns the base address of the TObject.
			 * @return Base address of the TObject
			 */
			virtual void* getBaseAddress()
			{
				return baseAddress;
			};

			/**
			 * Returns the address of the TObject.
			 * @return Address of the TObject
			 */
			virtual void* getAddress()
			{
				startAddress = (void*)((char*)getBaseAddress() + getOffset());

				if(ptrDepth == 2) {
					return startAddress;
				} else if(ptrDepth > 2) {
					void* result = startAddress;
					for(int i = ptrDepth; i > 2; i--) {
						if(result == nullptr) {
							throw std::invalid_argument(std::string("This address is a nullpointer: ") + getName());
						}
						result = (void*)(*(void**)result);
					}
					return result;
				} else {
					ptr = &startAddress;
					if(ptrDepth == 1) {
						return ptr;
					} else {
						return &ptr;
					}
				}
			};

			/**
			 * Makes a clone of the MetaInfo instance.
			 * @return Pointer to the cloned MetaInfo instance
			 */
			virtual MetaInfo* clone() = 0;
	};
}

#endif
