#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <string>

namespace rootJS
{
	/**
	 * Utility class for various purposes.
	 */
	class Toolbox
	{
		public:
			/**
			 * Throws a new v8 exception.
			 *
			 * @param message the exception message
			 */
			static void throwException(const std::string &message);
	};
}

#endif // TOOLBOX_H
