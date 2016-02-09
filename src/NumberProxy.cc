#include "NumberProxy.h"
#include "PointerInfo.h"
#include <v8.h>

#include "Toolbox.h"

namespace rootJS
{
	bool NumberProxy::isNumber(std::string type)
	{
		// TODO
		return false;
	}

	NumberProxy::NumberProxy(MetaInfo &info, TClass *scope): PrimitiveProxy(info, scope)
	{}

	NumberProxy::~NumberProxy() {
		if(backedUp) {
			free(*(void**)getAddress());
			free(getAddress());
		}
	}

	v8::Local<v8::Value> NumberProxy::get()
	{
		if(getAddress())
		{
			return v8::Number::New(v8::Isolate::GetCurrent(), castToDouble(getAddress()));
		}
		return getProxy();
	}

	void NumberProxy::backup()
	{
		void **ptrptr = (void**)malloc(sizeof(void*));
		void *numberPtr = 0;
		switch(numberType) {
			#define BACKUP(numbertype, pointer) \
			case NumberType::numbertype:		\
				numberPtr = malloc(sizeof(pointer)); \
				*(pointer*)numberPtr = *((pointer*)getAddress()); \
				break;

				BACKUP(INT_T,Int_t)
				BACKUP(UINT_T,UInt_t)

				BACKUP(SHORT_T,Short_t)
				BACKUP(USHORT_T,UShort_t)

				BACKUP(DOUBLE_T,Double_t)
				BACKUP(LONGDOUBLE_T,LongDouble_t)

				BACKUP(LONG_T,Long_t)
				BACKUP(ULONG_T,ULong_t)

				BACKUP(LONG64_T,Long64_t)
				BACKUP(ULONG64_T,ULong64_t)

				BACKUP(FLOAT_T,Float_t)
		}
		*ptrptr = numberPtr;

		const char* typeName = info->getTypeName();
		delete info;
		info = new PointerInfo(ptrptr, typeName);
		backedUp = true;
	}

	Double_t NumberProxy::castToDouble(void *ptr) {
		switch(numberType)
		{

#define SWITCH_CAST_DOUBLE(numbertype, pointer)           \
        case NumberType::numbertype:                                \
            return *((pointer*)getAddress());                       \
            break;

			SWITCH_CAST_DOUBLE(INT_T,Int_t)
			SWITCH_CAST_DOUBLE(UINT_T,UInt_t)

			SWITCH_CAST_DOUBLE(SHORT_T,Short_t)
			SWITCH_CAST_DOUBLE(USHORT_T,UShort_t)

			SWITCH_CAST_DOUBLE(DOUBLE_T,Double_t)
			SWITCH_CAST_DOUBLE(LONGDOUBLE_T,LongDouble_t)

			SWITCH_CAST_DOUBLE(LONG_T,Long_t)
			SWITCH_CAST_DOUBLE(ULONG_T,ULong_t)

			SWITCH_CAST_DOUBLE(LONG64_T,Long64_t)
			SWITCH_CAST_DOUBLE(ULONG64_T,ULong64_t)

			SWITCH_CAST_DOUBLE(FLOAT_T,Float_t)

		default:
			Toolbox::throwException("Unknown number format in NumberProxy (castToDouble).");
			return -1;
		}
	}

#define ROOTJS_NUMBER_PROXY( datatype , numbertype )                   \
    ObjectProxy* NumberProxy::datatype##Construct(MetaInfo &info, TClass *scope) {   \
        NumberProxy* proxy = new NumberProxy(info, scope);                                 \
        proxy->numberType = NumberType::numbertype;                                             \
        return proxy;                                                                               \
    }

	ROOTJS_NUMBER_PROXY(int, INT_T )
	ROOTJS_NUMBER_PROXY(uint, UINT_T )

	ROOTJS_NUMBER_PROXY(short, SHORT_T )
	ROOTJS_NUMBER_PROXY(ushort, USHORT_T )

	ROOTJS_NUMBER_PROXY(double, DOUBLE_T)
	ROOTJS_NUMBER_PROXY(ldouble, LONGDOUBLE_T )

	ROOTJS_NUMBER_PROXY(long, LONG_T )
	ROOTJS_NUMBER_PROXY(ulong, ULONG_T )

	ROOTJS_NUMBER_PROXY(llong, LONG64_T )
	ROOTJS_NUMBER_PROXY(ullong, ULONG64_T )

	ROOTJS_NUMBER_PROXY(_int64, LONG64_T )
	ROOTJS_NUMBER_PROXY(u_int64, ULONG64_T )

	ROOTJS_NUMBER_PROXY(float, FLOAT_T )


	void NumberProxy::setValue(v8::Local<v8::Value> value)
	{
		if(isConst())
		{
			Toolbox::throwException("This value cannot be overwritten, it's constant.");
			return;
		}

		double numberValue;
		if(value->IsNumberObject())
		{
			numberValue = v8::Local<v8::NumberObject>::Cast(value)->NumberValue();
		}
		else if(value->IsNumber())
		{
			numberValue = v8::Local<v8::Number>::Cast(value)->Value();
		}
		else
		{
			Toolbox::throwException("This element can only store numbers.");
			return;
		}

#define SWITCH_SET_VALUE(numbertype, pointer)                 \
        case NumberType::numbertype:                            \
            *((pointer*)getAddress()) = (pointer)numberValue;    \
            break;

#define SWITCH_SET_VALUE_DOUBLE(numbertype)                 \
        case NumberType::numbertype:                            \
            *((double*)getAddress()) = numberValue;    \
            break;


		switch(numberType)
		{
			SWITCH_SET_VALUE(INT_T,int)
			SWITCH_SET_VALUE(UINT_T,unsigned int)

			SWITCH_SET_VALUE(SHORT_T,short)
			SWITCH_SET_VALUE(USHORT_T,unsigned short)

			SWITCH_SET_VALUE_DOUBLE(DOUBLE_T)
			SWITCH_SET_VALUE(LONGDOUBLE_T, long double)

			SWITCH_SET_VALUE(LONG_T,long)
			SWITCH_SET_VALUE(ULONG_T,unsigned long)

			SWITCH_SET_VALUE(LONG64_T,long long)
			SWITCH_SET_VALUE(ULONG64_T,unsigned long long)

			SWITCH_SET_VALUE(FLOAT_T,float)


		}
	}
}
