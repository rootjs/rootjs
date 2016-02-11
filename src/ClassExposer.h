#ifndef SRC_CLASSEXPOSER_H_
#define SRC_CLASSEXPOSER_H_

#include <TROOT.h>

namespace rootJS {

class ClassExposer {
public:
	virtual ~ClassExposer();
	ClassExposer();
	static void expose(TClass*);

};

} /* namespace rootJS */

#endif /* SRC_CLASSEXPOSER_H_ */
