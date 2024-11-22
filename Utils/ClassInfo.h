// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ClassInfo.generated.h"

struct PropertyInfo {
private:
    void* DefaultValue;

public:
    UFunction* Function;
    FProperty* Property;
    bool Required;

    PropertyInfo(UFunction* const Function, FProperty* const Property) : Function(Function), Property(Property), Required(false), DefaultValue(nullptr) {}
    ~PropertyInfo();
    void SetDefaultValue(void* defaultValue);
    void* GetDefaultValue() const { return DefaultValue; }

    void SetValueIn(UObject* container, void* value) const;
};

UCLASS()
class XD_API UClassInfo : public UObject {
    GENERATED_BODY()
    
protected:
    UPROPERTY()
    UClass* Class;

public:
    TMap<FString, PropertyInfo> Properties;

    UClass* GetBaseClass() const { return Class; }

    UClassInfo* Init(UClass* classL);
    PropertyInfo* LoadProperty(const FString& name);
};
