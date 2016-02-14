#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <string>
#include <v8.h>

namespace rootJS
{
	/**
	 * Utility class for various purposes.
	 */
	class Toolbox
	{
		public:
			static const int INTERNAL_FIELD_COUNT = 2;
			
			/**
			 * Controls the verbosity of the compiled module.
			 *
			 * 0 = off
			 *
			 * 1 = logging
			 *
			 * 2 = debug
			 */
			static const int VERBOSITY = 0;

			/**
			 * Enumerates the internal fields of v8::Objects.
			 */
			enum InternalFieldData { ObjectProxyPtr, PropertyMapPtr };

			/**
			 * Throws a new v8 exception.
			 *
			 * @param message the exception message
			 */
			static void throwException(const std::string &message);

			/**
			 *	Log the specified message at info level.
			 *
			 *	@param
			 *		the message to log
			 */
			static void logInfo(const std::string &message);

			/**
			 *	Log the specified message at error level.
			 *
			 *	@param
			 *		the message to log
			 */
			static void logError(const std::string &message);

			static std::string Stringv8toStd(v8::Local<v8::String> vstr);
	};
}

#endif
