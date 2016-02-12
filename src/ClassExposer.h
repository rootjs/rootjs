#ifndef SRC_CLASSEXPOSER_H_
#define SRC_CLASSEXPOSER_H_

#include <TROOT.h>
#include <v8.h>

namespace rootJS {

class ClassExposer {
public:
	virtual ~ClassExposer();

	static void expose(TClass*,v8::Local<v8::Object>);
	static std::vector<std::string> splitClassName(std::string name, std::vector<std::string>& vec);
private:
	ClassExposer();
};

} /* namespace rootJS */

#endif /* SRC_CLASSEXPOSER_H_ */
