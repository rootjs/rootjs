#ifndef SRC_MEMBERMODE_H_
#define SRC_MEMBERMODE_H_

#include <v8.h>

#include <TDataMember.h>
#include <TClassRef.h>
#include <TGlobal.h>

namespace RootJS {

class MemberMode : public ProxyMode {
public:
  MemberMode(TDataMember &type);
  ~MemberMode();

  virtual bool isGlobal();
  virtual Long_t GetOffset();

protected:
};
}
#endif
