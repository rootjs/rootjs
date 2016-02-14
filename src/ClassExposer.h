#ifndef SRC_CLASSEXPOSER_H_
#define SRC_CLASSEXPOSER_H_

#include <TROOT.h>
#include <v8.h>

namespace rootJS {

class ClassExposer {
public:
	virtual ~ClassExposer();

    /**
     * This method exposes all the contents of the given TClass so that rootJS has access to the contents.
     * The namespace hierarchy is preserved and mapped to the exported node.js object. Namespaces that don"t
     * have a TClass are generated as stubs and inserted at the appropriate position.
     *
     * @param clazz
     *              The class which will be exposed
     * @param exports
     *              The exports Local<Object> will be examined to see if the class has already has been exposed.
     *              If it hasn't been exposed, then the ClassExposer will expose the contents of the class.
     */
	static void expose(TClass*,v8::Local<v8::Object>) throw(std::invalid_argument);

private:
	ClassExposer();
	/**
     * Splits the given class name with the delimiter "::" and inserts the split up parts
     * in a std::vector<std::string>
     *
     * @param The name to be split
     * @param The vector which will have the split up names and then be returned
     */
	static std::vector<std::string> splitClassName(std::string name, std::vector<std::string>& vec);
};

} /* namespace rootJS */

#endif /* SRC_CLASSEXPOSER_H_ */
