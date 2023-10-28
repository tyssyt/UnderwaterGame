// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaturalResourceUI.h"
#include "Components/TextBlock.h"
#include "XD/SelectedUI.h"
#include "XD/Buildings/BuildingUI.h"
#include "XD/Construction/ConstructionMaterialsUI.h"
#include "XD/Resources/ResourceAmountUI.h"
#include "NaturalResourceSelectedUI.generated.h"

UCLASS(Abstract)
class XD_API UNaturalResourceSelectedUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UNaturalResourceUI* NaturalResource;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Name;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBuildingUI* Building;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UConstructionMaterialsUI* ConstructionMaterials;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceAmountUI> ResourceAmountUIClass;

public:
    UPROPERTY()
    ANaturalResourceActor* Selected;
    
    UNaturalResourceSelectedUI* Init(ANaturalResourceActor* actor);

    virtual void Tick() override;

    UFUNCTION(BlueprintCallable)
    void OnClickConstruct();
};
