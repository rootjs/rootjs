#ifndef SRC_GLOBALMODE_H_
#define SRC_GLOBALMODE_H_



namespace RootJS {

class GlobalMode: public ProxyMode{
protected:
	GlobalMode(TDataMember& type);
};}

#endif /* SRC_GLOBALMODE_H_ */
