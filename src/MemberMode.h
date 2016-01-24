#ifndef SRC_OBJECTPROXY_H_
#define SRC_OBJECTPROXY_H_



#include <v8.h>

#include <TDataMember.h>
#include <TClassRef.h>
#include <TGlobal.h>

namespace RootJS {

class MemberMode: public ProxyMode{
protected:
	MemberMode(TDataMember& type);


};}
