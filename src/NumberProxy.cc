#include "NumberProxy.h"
#include <iostream>
#include <v8.h>

namespace rootJS {
	bool NumberProxy::isNumber(std::string type)
	{
		// TODO
		return false;
	}

	NumberProxy::NumberProxy(const TDataMember& type, TClassRef scope): PrimitiveProxy(type, scope) {

	}

	NumberProxy::NumberProxy(const TGlobal & type, TClassRef scope): PrimitiveProxy(type, scope) {

	}

	v8::Local<v8::Value> NumberProxy::get() {
		if(getAddress()) {
			return v8::Number::New(v8::Isolate::GetCurrent(), castToDouble(getAddress()));
		}
		return getProxy();
	}

	Double_t NumberProxy::castToDouble(void *ptr) {
		switch(numberType) {

#define SWITCH_CAST_DOUBLE(numbertype, pointer)           \
        case NumberType::numbertype:                                \
            return *((pointer*)getAddress());                       \
            break;

			SWITCH_CAST_DOUBLE(INT_T,Int_t)
			SWITCH_CAST_DOUBLE(DOUBLE_T,Double_t)
			SWITCH_CAST_DOUBLE(SHORT_T,Short_t)
			SWITCH_CAST_DOUBLE(USHORT_T,UShort_t)

			SWITCH_CAST_DOUBLE(UINT_T,UInt_t)

			SWITCH_CAST_DOUBLE(LONG_T,Long_t)
			SWITCH_CAST_DOUBLE(ULONG_T,ULong_t)

			SWITCH_CAST_DOUBLE(LONGDOUBLE_T,LongDouble_t)

			SWITCH_CAST_DOUBLE(LONG64_T,Long64_t)
			SWITCH_CAST_DOUBLE(ULONG64_T,ULong64_t)

			SWITCH_CAST_DOUBLE(FLOAT_T,Float_t)

		default:
			v8::Isolate::GetCurrent()->ThrowException(
			    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
			                            "Unknown number format in NumberProxy (castToDouble)."
			                           ));
			return -1;
		}
	}

#define ROOTJS_NUMBER_PROXY( datatype , numbertype )                   \
    ObjectProxy* NumberProxy::datatype##Construct(const TDataMember& type, TClassRef scope) {   \
        NumberProxy* proxy = new NumberProxy(type, scope);                                 \
        proxy->numberType = NumberType::numbertype;                                             \
        return proxy;                                                                               \
    }

#define ROOTJS_NUMBER_PROXY_2( datatype , numbertype )                   \
    ObjectProxy* NumberProxy::datatype##Construct(const TGlobal& type, TClassRef scope) {   \
		NumberProxy* proxy = new NumberProxy(type, scope);                              	\
        proxy->numberType = NumberType::numbertype;                                             \
        return proxy;                                                                               \
    }

	ROOTJS_NUMBER_PROXY(int, INT_T )
	ROOTJS_NUMBER_PROXY_2(int, INT_T )
	ROOTJS_NUMBER_PROXY(double, DOUBLE_T)
	ROOTJS_NUMBER_PROXY_2(double, DOUBLE_T)

	ROOTJS_NUMBER_PROXY(short, SHORT_T )
	ROOTJS_NUMBER_PROXY(ushort, USHORT_T )

	ROOTJS_NUMBER_PROXY_2(short, SHORT_T )
	ROOTJS_NUMBER_PROXY_2(ushort, USHORT_T )


	ROOTJS_NUMBER_PROXY(uint, UINT_T )


	ROOTJS_NUMBER_PROXY(long, LONG_T )
	ROOTJS_NUMBER_PROXY(ulong, ULONG_T )

	ROOTJS_NUMBER_PROXY(llong, LONG64_T )
	ROOTJS_NUMBER_PROXY(ullong, ULONG64_T )

	ROOTJS_NUMBER_PROXY(float, FLOAT_T )

	ROOTJS_NUMBER_PROXY(_int64, LONG64_T )
	ROOTJS_NUMBER_PROXY(u_int64, ULONG64_T )

	ROOTJS_NUMBER_PROXY(ldouble, LONGDOUBLE_T )

	//Constructor number 2

	ROOTJS_NUMBER_PROXY_2(uint, UINT_T )

	ROOTJS_NUMBER_PROXY_2(float, FLOAT_T )

	ROOTJS_NUMBER_PROXY_2(ldouble, LONGDOUBLE_T )

	ROOTJS_NUMBER_PROXY_2(long, LONG_T )
	ROOTJS_NUMBER_PROXY_2(ulong, ULONG_T )

	ROOTJS_NUMBER_PROXY_2(_int64, LONG64_T )
	ROOTJS_NUMBER_PROXY_2(u_int64, ULONG64_T )

	ROOTJS_NUMBER_PROXY_2(llong, LONG64_T )
	ROOTJS_NUMBER_PROXY_2(ullong, ULONG64_T )

	void NumberProxy::setValue(v8::Local<v8::Value> value) {
		if(isConst()) {
			v8::Isolate::GetCurrent()->ThrowException(
			    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
			                            "This value cannot be overwritten, it's constant."
			                           ));
		}
		double numberValue;
		if(value->IsNumberObject()) {
			numberValue = v8::Local<v8::NumberObject>::Cast(value)->NumberValue();
		} else if(value->IsNumber()) {
			numberValue = v8::Local<v8::Number>::Cast(value)->Value();
		} else {
			v8::Isolate::GetCurrent()->ThrowException(
			    v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
			                            "This element can only store numbers."
			                           ));
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


		switch(numberType) {
			SWITCH_SET_VALUE(INT_T,int)
			SWITCH_SET_VALUE_DOUBLE(DOUBLE_T)

			SWITCH_SET_VALUE(SHORT_T,short)
			SWITCH_SET_VALUE(USHORT_T,unsigned short)

			SWITCH_SET_VALUE(UINT_T,unsigned int)

			SWITCH_SET_VALUE(LONG_T,long)
			SWITCH_SET_VALUE(ULONG_T,unsigned long)

			SWITCH_SET_VALUE_DOUBLE(LONGDOUBLE_T)

			SWITCH_SET_VALUE(LONG64_T,long long)
			SWITCH_SET_VALUE(ULONG64_T,unsigned long long)

			SWITCH_SET_VALUE(FLOAT_T,float)


		}
	}
}
