// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MainUI.h"
#include "XD/NaturalResources/HardOreDeposit.h"
#include "XD/NaturalResources/ConductiveOreDeposit.h"
#include "XD/NaturalResources/OilFish.h"
#include "XD/Buildings/HardOreMine.h"
#include "XD/Buildings/ConductiveOreMine.h"
#include "XD/Buildings/OilFishHarvester.h"
#include "XD/Buildings/Depot.h"
#include "XD/Buildings/Smelter.h"
#include "XD/Buildings/Conveyor.h"
#include "Buildings/Habitat.h"
#include "Buildings/WorkerHouse.h"
#include "XD/Buildings/PickupPad.h"

#include "CoreMinimal.h"
#include "ImageUI.h"
#include "BlueprintHolder.generated.h"


UCLASS(Abstract, Blueprintable)
class XD_API UBlueprintHolder : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMainUI> MainUIClass;
    UPROPERTY()
    UMainUI* MainUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHardOreDepositUI> HardOreDepositUIClass;
    UPROPERTY()
    UHardOreDepositUI* HardOreDepositUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConductiveOreDepositUI> ConductiveOreDepositUIClass;
    UPROPERTY()
    UConductiveOreDepositUI* ConductiveOreDepositUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UOilFishUI> OilFishUIClass;
    UPROPERTY()
    UOilFishUI* OilFishUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHardOreMineUI> HardOreMineUIClass;
    UPROPERTY()
    UHardOreMineUI* HardOreMineUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConductiveOreMineUI> ConductiveOreMineUIClass;
    UPROPERTY()
    UConductiveOreMineUI* ConductiveOreMineUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UOilFishHarvesterUI> OilFishHarvesterUIClass;
    UPROPERTY()
    UOilFishHarvesterUI* OilFishHarvesterUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDepotUI> DepotUIClass;
    UPROPERTY()
    UDepotUI* DepotUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConveyorUI> ConveyorUIClass;
    UPROPERTY()
    UConveyorUI* ConveyorUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<USmelterUI> SmelterUIClass;
    UPROPERTY()
    USmelterUI* SmelterUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHabitatUI> HabitatUIClass;
    UPROPERTY()
    UHabitatUI* HabitatUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UWorkerHouseUI> WorkerHouseUIClass;
    UPROPERTY()
    UWorkerHouseUI* WorkerHouseUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPickupPadUI> PickupPadUIClass;
    UPROPERTY()
    UPickupPadUI* PickupPadUI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UImageUI> ImageUIClass;

    void Init(APlayerController* controller);
    USelectedUI* GetUI(AActor* actor) const;

};
