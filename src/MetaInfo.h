#ifndef SRC_METAINFO_H_
#define SRC_METAINFO_H_
#include <RConfig.h>
#include <TDataMember.h>
#include <TClassRef.h>
#include <TGlobal.h>
#include <RConfig.h>
namespace rootJS {

	/**
	 * This class encapsulates the differences in behaviour between TMember and
	 * TGlobal
	 * */
	class MetaInfo {
		protected:
			void* baseAddress;
		public:
			MetaInfo(const TObject &foo, void *baseAddress) {
				this->baseAddress = baseAddress;
			};
			virtual ~MetaInfo() {};

            /**
             * Checks if the TObject is global.
             * @return If the TObject is global
             */
			virtual bool isGlobal() {
				return false;
			};
			virtual Long_t getOffset() {
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
             * Returns the base address of the TObject.
             * @return Base address of the TObject
             */
			virtual void* getBaseAddress() {
				return baseAddress;
			};

			/**
             * Returns the address of the TObject.
             * @return Address of the TObject
             */
			virtual void* getAddress() {
				return (void*)((char*)getBaseAddress() + getOffset());
			}

            /**
             * Makes a clone of the MetaInfo instance.
             * @return Pointer to the cloned MetaInfo instance
             */
			virtual MetaInfo* clone() = 0;

		protected:
		private:
	};
}

#endif /* SRC_METAINFO_H_ */
