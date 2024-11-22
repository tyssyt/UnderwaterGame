// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Cheats.h"
#include "TickTimer.h"
#include "Construction/ConstructionManager.h"
#include "Electricity/ElectricityManager.h"
#include "Encyclopedia/Encyclopedia.h"
#include "Engine/GameInstance.h"
#include "Map/Map.h"
#include "PopulationManager/BiomePopulationManager.h"
#include "GameInstanceX.generated.h"

UCLASS()
class XD_API UGameInstanceX : public UGameInstance {
    GENERATED_BODY()

    virtual void Init() override;

public:
    UGameInstanceX();

    // TODO we could also consider using subsystems for some of the below things
    // https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/Subsystems/

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UEncyclopedia* TheEncyclopedia;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UConstructionManager* TheConstructionManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UElectricityManager* TheElectricityManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBiomePopulationManager* ThePopulationManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UMap* TheMap;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCheats* TheCheats;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTickTimer* TheTickTimer;
};
