// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resources/Resource.h"
#include "UObject/Object.h"
#include "ComponentLoader.generated.h"

class ABuilding;
struct Material;
class UComponentLoader;

struct PropertyInfo {
    UFunction* Function;
    FProperty* Property;
    bool Required;
    void* DefaultValue;
};

struct XD_API MaterialRef {
    MaterialRef(UResource* resource, PropertyInfo* amountRef) : Resource(resource), AmountRef(amountRef) {}
    UResource* Resource;
    PropertyInfo* AmountRef;
};

UCLASS()
class XD_API UComponentInfo : public UObject {
    GENERATED_BODY()

public:
    virtual ~UComponentInfo() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TSubclassOf<UActorComponent> ComponentClass;

    TMap<FString, PropertyInfo> Properties;

    // TODO consider renaming, because need is now used for the population growth needs
    TArray<MaterialRef> Needs;
    
    UComponentInfo* Init(const FText& name, const TSubclassOf<UActorComponent> componentClass);
    void AddProperty(const FString& name, UFunction* setter, FProperty* prop, bool required, void* defaultValue);
    void AddNeed(UResource* resource, PropertyInfo* amountRef);
};

UCLASS()
class XD_API UComponentLoader : public UObject {
    GENERATED_BODY()

protected:
    TMap<PropertyInfo*, void*> PropertyValues;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UComponentInfo* ComponentInfo;

    UComponentLoader* Init(UComponentInfo* componentInfo);
    virtual ~UComponentLoader() override;

    void SetProperty(PropertyInfo* property, void* value);

    UActorComponent* AddComponentToBuilding(ABuilding* building) const;

    TArray<Material> GetNeeds();

    template<class T>
    T* GetValue(PropertyInfo* property) {
        return static_cast<T*>(*PropertyValues.Find(property));
    }
};
