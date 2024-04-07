// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Construction/ConstructionOptions.h"
#include "ComponentX.generated.h"

class UBuildingSelectedUI;
class UBuildingSelectedUIComponent;
class UComponentInfo;
class UBuilderModeExtension;

UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UComponentX : public UActorComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UComponentInfo* ComponentInfo;

public:
    UComponentX* Init(UComponentInfo* componentInfo);
    
    virtual TSubclassOf<UBuilderModeExtension> GetBuilderModeExtension() const {
        return nullptr;
    }
    
    virtual void OnConstructionComplete(UConstructionOptions* options) {}

    virtual void AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {}
};
