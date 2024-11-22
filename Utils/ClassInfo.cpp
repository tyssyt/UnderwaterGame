// Fill out your copyright notice in the Description page of Project Settings.


#include "ClassInfo.h"

PropertyInfo::~PropertyInfo() {
    if (DefaultValue && Property->IsA<FObjectProperty>())
        static_cast<UObject*>(DefaultValue)->RemoveFromRoot();
    else {
        // TODO should I not free the value?
    }
}

void PropertyInfo::SetDefaultValue(void* defaultValue) {
    if (defaultValue && Property->IsA<FObjectProperty>())
        static_cast<UObject*>(defaultValue)->AddToRoot();
    DefaultValue = defaultValue;
}

void CopyValue(FProperty* prop, void* container, void* val) {
    if (prop->IsA<FObjectProperty>())
        prop->CopySingleValue(prop->ContainerPtrToValuePtr<void>(container), &val);
    else
        prop->CopySingleValue(prop->ContainerPtrToValuePtr<void>(container), val);
}
void CallSingleParamFunction(UFunction* function, UObject* self, FProperty* param, void* arg) {
    uint8* params = static_cast<uint8*>(FMemory_Alloca(function->ParmsSize));
    FMemory::Memzero(params, function->ParmsSize);
    CopyValue(param, params, arg);

    self->ProcessEvent(function, params);
}
void PropertyInfo::SetValueIn(UObject* container, void* value) const {
    if (Function)
        CallSingleParamFunction(Function, container, Property, value);
    else
        CopyValue(Property, container, value);
}

UClassInfo* UClassInfo::Init(UClass* classL) {
    Class = classL;
    return this;
}

PropertyInfo* UClassInfo::LoadProperty(const FString& name) {
    for (TFieldIterator<UFunction> it(Class, EFieldIterationFlags::IncludeSuper); it; ++it) {
        const auto rawFunc = *it;
        if (!rawFunc->GetName().Equals(TEXT("Set") + name))
            continue;

        FProperty* param = nullptr;
        for (TFieldIterator<FProperty> paramIt(rawFunc); paramIt; ++paramIt) {
            if (!(paramIt->PropertyFlags & CPF_ReturnParm) && paramIt->PropertyFlags & CPF_Parm) {
                if (param) {
                    param = nullptr;
                    break;
                }
                param = *paramIt;
            }
        }
        if (!param)
            continue;

        return &Properties.Emplace(name, PropertyInfo(rawFunc, param));
    }

    for (TFieldIterator<FProperty> it(Class, EFieldIterationFlags::IncludeSuper); it; ++it) {
        const auto rawProp = *it;
        if (!rawProp->GetName().Equals(name))
            continue;

        return &Properties.Emplace(name, PropertyInfo(nullptr, rawProp));
    }

    return nullptr;
}
