#ifndef TYPES_H
#define TYPES_H

#include "MetaInfo.h"

#include <map>

#include <nan.h>
#include <TDataType.h>
#include <TCollection.h>

namespace rootJS
{
	class Types
	{
		public:
			/**
			 * Gets the normalized (no typedef, ...) TDataType which matches name
			 * @return nullpointer if there is no matching datatype
			 */
			static TDataType* getTypeByName(const std::string &name);

			/**
			 *	Use Types::getTypeByName(info.getTypeName()) to get the normalized TDataType.
			 *	The normalized name is than stored in the supplied trueType string.
			 *	Note that trueType does not change, if the type could not be resolved.
			 *
			 *	@param info
			 *			the type to get the normalized type name from
			 *
			 *	@param trueType
			 *			the storage for the resolved type name
			 *
			 *	@return true if info.getTypeName() name was resolved
			 */
			static bool resolveTypeName(MetaInfo &info, std::string &trueType);

			/**
			 *	Use Types::getTypeByName(info.getTypeName()) to get the normalized TDataType.
			 *	The normalized name is than stored in the supplied trueType string.
			 *	Note that trueType does not change, if the type could not be resolved.
			 *
			 *	@param stdTypeName
			 *			the unnormalized type name
			 *
			 *	@param trueType
			 *			the storage for the resolved type name
			 *
			 *	@return true if info.getTypeName() name was resolved
			 */
			static bool resolveTypeName(std::string stdTypeName, std::string &trueType);

			/*
			 *  Check if the address of the specified enum constant is contained in the specified collection of enums.
			 *
			 *	@paran info
			 *			the enum constant to check
			 *
			 *	@param enumCollection
			 *			the collection of enums in question
			 *
			 *	@return true if the address was found within the specified collection
			 *
			 * static bool isDuplicateConstant(MetaInfo &info, TCollection* enumCollection);
			 */

			/**
			 * Checks if the given value is an v8 Boolean.
			 *
			 * @param value
			 *              The value to checked
			 */
			static bool isV8Boolean(v8::Local<v8::Value> value);
			/**
			 * Checks if the given value is an v8 Number.
			 *
			 * @param value
			 *              The value to checked
			 */
			static bool isV8Number(v8::Local<v8::Value> value);
			/**
			 * Checks if the given value is an v8 String.
			 *
			 * @param value
			 *              The value to checked
			 */
			static bool isV8String(v8::Local<v8::Value> value);
			/**
			 * Checks if the given value is an v8 Primitive.
			 *
			 * @param value
			 *              The value to checked
			 */
			static bool isV8Primitive(v8::Local<v8::Value> value);

			static const std::map<std::string, std::string> operatorNames;
	};
}

#endif
