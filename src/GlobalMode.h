#ifndef SRC_GLOBALMODE_H_
#define SRC_GLOBALMODE_H_
#include "ProxyMode.h"

namespace rootJS {

	class GlobalMode: public ProxyMode {
	public:
		GlobalMode(const TGlobal & type);
		~GlobalMode();
		virtual bool isGlobal();
		virtual Long_t GetOffset();

		virtual bool isConst();
		virtual bool isStatic() { return true; };
		virtual const char* getTypeName();

		const TGlobal &currentObject;
	protected:
	};
}

#endif /* SRC_GLOBALMODE_H_ */
