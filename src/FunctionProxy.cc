#include "FunctionProxy.h"
#include "ObjectProxy.h"

#include <map>
#include <string>
#include <vector>

#include <v8.h>

#include <TClassRef.h>
#include <TFunction.h>

namespace RootJS {
    std::map<TFunction*, CallFunc_t*> FunctionProxy::functions;

    CallFunc_t* FunctionProxy::getCallFunc(TFunction* method)
    {
        std::map<TFunction*, CallFunc_t*>::iterator iterator = FunctionProxy::functions.find(method);
        if (iterator != FunctionProxy::functions.end())
        {
            return iterator->second;
        }
        else
        {
            // TODO handle TFunction not in functions cache
        }
    }

    std::vector<TFunction*> FunctionProxy::getMethodsFromName(TClassRef scope, std::string name)
    {
        // TODO
    }

    FunctionProxy::FunctionProxy(void* address, TFunction function, TClassRef scope)
        : Proxy(address, function, scope)
    {
        // TODO
    }

    const TFunction& FunctionProxy::getType()
    {
        return dynamic_cast<const TFunction&>(Proxy::getType());
    }

    /*ObjectProxy* FunctionProxy::validateArgs(v8::FunctionCallbackInfo args)
    {
        // TODO
    }*/

    ObjectProxy FunctionProxy::call(ObjectProxy args[]) const
    {
        // TODO
    }

    bool FunctionProxy::processCall(TFunction* method, void* args, void* self, void* result)
    {
        // TODO
    }

    void* FunctionProxy::callConstructor(TFunction* method, TClassRef type, void* args)
    {
        // TODO
    }

    void FunctionProxy::callDestructor(TClassRef type, void* self)
    {
        // TODO
    }

    void* FunctionProxy::callObject(TFunction* method, void* self, void* args, TClassRef resType)
    {
        // TODO
    }

    template <typename T>
    T FunctionProxy::callPrimitive(TFunction* method, void* self, void* args)
    {
        // TODO
    }
}
