#ifndef SRC_MEMBERMODE_H_
#define SRC_MEMBERMODE_H_

#include <v8.h>
#include "ProxyMode.h"
#include <TDataMember.h>
#include <TClassRef.h>
#include <TGlobal.h>

namespace RootJS {

class MemberMode : public ProxyMode {
public:
  MemberMode(const TDataMember &);
  ~MemberMode();

  virtual bool isGlobal();
  virtual Long_t GetOffset();
  const TDataMember * currentObject;
protected:
};
}
#endif
