// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Need.h"
#include "HabitatPopulationManager.generated.h"

class AHabitat;

UCLASS()
class XD_API UHabitatPopulationManager : public UObject {
    GENERATED_BODY()

public:
    UHabitatPopulationManager* Init(AHabitat* habitat);

    void NotifyBuildingsChanged() { BuildingsChangedNotificationReceived = true; };
    void TickPopulation();

    int GetMaxPop() const { return MaxPop; }
    int GetCurrentPop() const { return CurrentPop; }
    int GetWorkforce() const { return Workforce; }
    TArray<UNeed*>& GetSatisfiedNeeds() { return SatisfiedNeedsLastTick; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    AHabitat* Habitat;

    int Tick;
    bool BuildingsChangedNotificationReceived;

    int MaxPop;
    int CurrentPop;
    int Workforce;

    int ConsecutiveGrowthTicks;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<UNeed*> SatisfiedNeedsLastTick;

    void UpdateMaxPop();
    int SatisfyNeeds();
    bool AdjustPop(int satisfiedPop);
    void RelocatePop() const;
    void ComputeWorkforce();
};
