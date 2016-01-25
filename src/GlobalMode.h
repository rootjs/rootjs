#ifndef SRC_GLOBALMODE_H_
#define SRC_GLOBALMODE_H_

namespace RootJS {

class GlobalMode : public ProxyMode {
public:
  GlobalMode(TGlobal &type);
  ~GlobalMode();
  virtual bool isGlobal();
  virtual Long_t GetOffset();

protected:
};
}

#endif /* SRC_GLOBALMODE_H_ */
