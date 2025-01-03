// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainUI.h"
#include "XD/Buildings/Conveyor.h"

#include "CoreMinimal.h"
#include "ImageUI.h"
#include "TextUI.h"
#include "Buildings/BuildingSelectedUI.h"
#include "Buildings/ConditionListUI.h"
#include "Buildings/Substation.h"
#include "Construction/ConstructionSiteUI.h"
#include "Construction/ConstructionUI.h"
#include "Electricity/PowerOverlayUI.h"
#include "Encyclopedia/EncyclopediaUI.h"
#include "Inventory/InventoryUI.h"
#include "Map/MapUI.h"
#include "NaturalResources/NaturalResourceSelectedUI.h"
#include "PopulationManager/NeedsSummaryUI.h"
#include "Recipes/RecipeSelectorUI.h"
#include "BlueprintHolder.generated.h"

UCLASS(Abstract, Blueprintable)
class XD_API UBlueprintHolder : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMainUI> MainUIClass;
    UPROPERTY(BlueprintReadOnly)
    UMainUI* MainUI;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaUI> EncyclopediaUIClass;
    UPROPERTY(BlueprintReadOnly)
    UEncyclopediaUI* EncyclopediaUI;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMapUI> MapUIClass;
    UPROPERTY(BlueprintReadOnly)
    UMapUI* MapUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNaturalResourceSelectedUI> NaturalResourceUIClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBuildingSelectedUI> BuildingUIClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConveyorUI> ConveyorUIClass;
    UPROPERTY(BlueprintReadOnly)
    UConveyorUI* ConveyorUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPowerOverlayUI> PowerOverlayUIClass;
    UPROPERTY(BlueprintReadOnly)
    UPowerOverlayUI* PowerOverlayUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConstructionUI> ConstructionUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConstructionSiteUI> ConstructionSiteUIClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEventUI> UEventUIClass;

    // TODO move into parent blueprint, also consider implementing in pure c++
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<USubstationUI> SubstationUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<URecipeSelectorUI> RecipeSelectorUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UImageUI> ImageUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConditionListUI> ConditionListUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTextUI> TextUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPowerUI> PowerUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceAmountUI> ResourceAmountUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceBalanceUI> ResourceBalanceUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UInventoryUI> InventoryUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UInventorySlotUI> InventorySlotUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNeedsSummaryUI> NeedsSummaryUIClass;

    void Init(APlayerController* controller);
    USelectedUI* GetUI(AXActor* actor) const;
};
