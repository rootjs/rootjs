#include "Types.h"

#include "Toolbox.h"

#include <nan.h>

#include <TROOT.h>
#include <TString.h>
#include <TEnum.h>
#include <TEnumConstant.h>

namespace rootJS
{

	TDataType* Types::getTypeByName(const std::string &stdTypeName)
	{
		std::string cpyTypeName = stdTypeName;
		// check if typeName starts with const
		std::size_t idx = cpyTypeName.find("const ");
		if(cpyTypeName.find('*') != std::string::npos)
		{
			cpyTypeName = cpyTypeName.substr(0, cpyTypeName.find('*'));
		}
		if(idx != std::string::npos && idx == 0)
		{
			return (TDataType*) (gROOT->GetListOfTypes(kTRUE)->FindObject(stdTypeName.substr(6).c_str()));
		}

		return (TDataType*) (gROOT->GetListOfTypes(kTRUE)->FindObject(stdTypeName.c_str()));
	}

	bool Types::resolveTypeName(MetaInfo &info, std::string &trueType)
	{
		return resolveTypeName(info.getTypeName(), trueType);
	}

	bool Types::resolveTypeName(std::string stdTypeName, std::string &trueType)
	{
		TDataType* type = Types::getTypeByName(stdTypeName);
		if(type == nullptr)
		{
			return false;
		}

		TString typeName = type->GetTypeName().Data();
		if(typeName.IsNull())
		{
			return false;
		}

		trueType = std::string(typeName.Data());
		return true;
	}

	/*
		bool Types::isDuplicateConstant(MetaInfo &info, TCollection* enumCollection)
		{
			if(enumCollection == nullptr)
			{
				return false;
			}

			TEnum* eNum = TEnum::GetEnum(info.getTypeName());
			if(eNum == nullptr || !eNum->IsValid())
			{
				return false;
			}

			TIter enumIter(enumCollection);
			TEnum *comp = nullptr;
			bool found = false;
			while ((comp = (TEnum*) enumIter()))
			{
				if (comp == nullptr || !comp->IsValid())
				{
					continue;
				}
				else if(eNum == comp)
				{
					found = true;
					break;
				}
			}

			if(!found)
			{
				return false;
			}

			TIter eConstIter(eNum->GetConstants());
			TEnumConstant *eConst = nullptr;
			std::string varName(info.getName());
			while ( (eConst = (TEnumConstant*) eConstIter()))
			{
				if(!eConst->IsValid() || eConst->GetValue() != *((Long64_t*) info.getAddress()))
				{
					continue;
				}

				if(eConst->GetAddress() == (void*) info.getAddress()) // will not happen
				{
					Toolbox::logInfo("equal address of constant '" + std::string(eConst->GetName())
					                 + "' and member '" + std::string(info.getName())
					                 + "' in scope '" + std::string(comp->GetQualifiedName()) + "'.", 1);
					return true;
				}
				else if(varName.compare(eConst->GetName()) == 0)
				{
					Toolbox::logInfo("equal name of constant '" + std::string(eConst->GetName())
					                 + "' and member '" + std::string(info.getName())
					                 + "' in scope '" + std::string(comp->GetQualifiedName()) + "'.", 1);
					return false; // return true
				}
				else
				{
					Toolbox::logInfo("equal value of constant '" + std::string(eConst->GetName())
					                 + "' and member '" + std::string(info.getName())
					                 + "' in scope '" + std::string(comp->GetQualifiedName()) + "'.", 1);
				}
			}

			return false;
		}
		*/

	bool Types::isV8Boolean(v8::Local<v8::Value> value)
	{
		return value->IsBoolean() || value->IsBooleanObject();
	}

	bool Types::isV8Number(v8::Local<v8::Value> value)
	{
		return value->IsNumber() || value->IsNumberObject();
	}

	bool Types::isV8String(v8::Local<v8::Value> value)
	{
		return value->IsString() || value->IsStringObject();
	}

	bool Types::isV8Primitive(v8::Local<v8::Value> value)
	{
		return isV8Boolean(value) || isV8Number(value) || isV8String(value);
	}


	const std::map<std::string, std::string> Types::operatorNames =
	    {
	        {"operator==", "_equals"
	        },
	        {"operator+=", "_setAdd"},
	        {"operator-=", "_setSubstract"},
	        {"operator*=", "_setMultiply"},
	        {"operator/=", "_setDivide"},
	        {"operator++", "_increment"},
	        {"operator--", "_decrement"},

	        {"operator+", "_add"},
	        {"operator-", "_substract"},
	        {"operator*", "_multiply"},
	        {"operator/", "_divide"},

	        {"operator<<=", "_setLeftshift"},
	        {"operator>>=", "_setRightshift"},

	        {"operator<<", "_leftshift"},
	        {"operator>>", "_rightshift"},

	        {"operator=", "_set"},
	        {"operator new", "_construct"},
	        {"operator delete", "_destruct"},

	        {"operator()", "_call"},
	        {"operator->", "_derefer"},

	        {"operator[]", "_at"},
	        {"operator delete[]", "_deleteArray"},
	        {"operator new[]", "_newArray"},

	        {"operator&=", "_setAnd"},
	        {"operator!=", "_setNot"},
	        {"operator|=", "_setOr"},
	        {"operator^=", "_setXOR"},

	        {"operator&", "_and"},
	        {"operator!", "_not"},
	        {"operator|", "_or"},
	        {"operator^", "_XOR"},
	        {"operator~", "_bitNot"},

	        {"delete[]", "_freeArray"},
	        {"delete[]", "_freeArray"},
	        {"delete[]", "_freeArray"},
	        {"delete[]", "_freeArray"},
	        {"delete[]", "_freeArray"},
	    };
}
