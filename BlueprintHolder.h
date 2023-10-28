// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainUI.h"
#include "XD/Buildings/HardOreMine.h"
#include "XD/Buildings/ConductiveOreMine.h"
#include "XD/Buildings/OilFishHarvester.h"
#include "XD/Buildings/Depot.h"
#include "XD/Buildings/Smelter.h"
#include "XD/Buildings/Conveyor.h"
#include "Buildings/Habitat.h"
#include "Buildings/WorkerHouse.h"
#include "XD/Buildings/PickupPad.h"
#include "XD/Buildings/AssemblyLine.h"

#include "CoreMinimal.h"
#include "ImageUI.h"
#include "TextUI.h"
#include "Buildings/Excavator.h"
#include "Buildings/Substation.h"
#include "Construction/ConstructionUI.h"
#include "Electricity/PowerOverlay.h"
#include "Encyclopedia/EncyclopediaUI.h"
#include "NaturalResources/NaturalResourceSelectedUI.h"
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
    TSubclassOf<UNaturalResourceSelectedUI> NaturalResourceUIClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHardOreMineUI> HardOreMineUIClass;
    UPROPERTY(BlueprintReadOnly)
    UHardOreMineUI* HardOreMineUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConductiveOreMineUI> ConductiveOreMineUIClass;
    UPROPERTY(BlueprintReadOnly)
    UConductiveOreMineUI* ConductiveOreMineUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UOilFishHarvesterUI> OilFishHarvesterUIClass;
    UPROPERTY(BlueprintReadOnly)
    UOilFishHarvesterUI* OilFishHarvesterUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDepotUI> DepotUIClass;
    UPROPERTY(BlueprintReadOnly)
    UDepotUI* DepotUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConveyorUI> ConveyorUIClass;
    UPROPERTY(BlueprintReadOnly)
    UConveyorUI* ConveyorUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<USmelterUI> SmelterUIClass;
    UPROPERTY(BlueprintReadOnly)
    USmelterUI* SmelterUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHabitatUI> HabitatUIClass;
    UPROPERTY(BlueprintReadOnly)
    UHabitatUI* HabitatUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UWorkerHouseUI> WorkerHouseUIClass;
    UPROPERTY(BlueprintReadOnly)
    UWorkerHouseUI* WorkerHouseUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPickupPadUI> PickupPadUIClass;
    UPROPERTY(BlueprintReadOnly)
    UPickupPadUI* PickupPadUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAssemblyLineUI> AssemblyLineUIClass;
    UPROPERTY(BlueprintReadOnly)
    UAssemblyLineUI* AssemblyLineUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<USubstationUI> SubstationUIClass;
    UPROPERTY(BlueprintReadOnly)
    USubstationUI* SubstationUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UExcavatorUI> ExcavatorUIClass;
    UPROPERTY(BlueprintReadOnly)
    UExcavatorUI* ExcavatorUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPowerOverlayUI> PowerOverlayUIClass;
    UPROPERTY(BlueprintReadOnly)
    UPowerOverlayUI* PowerOverlayUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConstructionUI> ConstructionUIClass;

    // TODO move into parent blueprint, also consider implementing in pure c++
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UImageUI> ImageUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTextUI> TextUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPowerUI> PowerUIClass;

    void Init(APlayerController* controller);
    USelectedUI* GetUI(AXActor* actor) const;

};
