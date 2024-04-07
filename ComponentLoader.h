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
    FProperty* Property;
    bool Required;
};

struct XD_API MaterialRef {
    MaterialRef(UResource* resource, FIntProperty* amountRef) : Resource(resource), AmountRef(amountRef) {}
    UResource* Resource;
    FIntProperty* AmountRef;
};

UCLASS()
class XD_API UComponentInfo : public UObject {
    GENERATED_BODY()

public:    
    enum class AddPropertyResult { Success, UnknownProperty, DuplicateProperty };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TSubclassOf<UActorComponent> ComponentClass;

    TMap<FString, PropertyInfo> Properties;

    // TODO consider renaming, because need is now used for the population growth needs
    TArray<MaterialRef> Needs;
    
    UComponentInfo* Init(const FText& name, const TSubclassOf<UActorComponent> componentClass);
    AddPropertyResult AddProperty(const FString& name, bool required);
    void AddNeed(UResource* resource, FIntProperty* amountRef);
};

UCLASS()
class XD_API UComponentLoader : public UObject {
    GENERATED_BODY()

protected:
    TMap<FProperty*, void*> PropertyValues;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UComponentInfo* ComponentInfo;

    UComponentLoader* Init(UComponentInfo* componentInfo);
    virtual ~UComponentLoader() override;

    void SetProperty(FProperty* property, void* value);

    UActorComponent* AddComponentToBuilding(ABuilding* building) const;

    TArray<Material> GetNeeds();

    template<class T>
    T* GetValue(FProperty* property) {
        return static_cast<T*>(*PropertyValues.Find(property));
    }
};
