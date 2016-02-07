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
			 * Enumerates the internal fields of v8::Objects.
			 */
			enum v8ObjectInternalField { ObjectProxyPtr };

			/**
			 * Throws a new v8 exception.
			 *
			 * @param message the exception message
			 */
			static void throwException(const std::string &message);

			/**
			 *	Log the specified message.
			 *
			 *	@param
			 *		the message to log
			 */
			static void log(const std::string &message);
	};
}

#endif // TOOLBOX_H
