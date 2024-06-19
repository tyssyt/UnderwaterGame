// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HabitatPopulationManager.h"
#include "WorkforceComponent.h"
#include "UObject/Object.h"
#include "BiomePopulationManager.generated.h"

UCLASS()
class XD_API UBiomePopulationManager : public UObject, public FTickableGameObject {
    GENERATED_BODY()

protected:
    uint32 LastFrameNumberWeTicked = INDEX_NONE;

    UPROPERTY()
    TArray<UHabitatPopulationManager*> Habitats;
    UPROPERTY()
    TArray<UWorkforceComponent*> Buildings;

public:
    UPROPERTY()
    int Population;
    UPROPERTY()
    int Workforce;
    UPROPERTY()
    int WorkforceRequired;

    int GetSurplusWorkforce() const { return Workforce - WorkforceRequired; }

    void AddHabitat(UHabitatPopulationManager* habitat) { Habitats.Add(habitat); }
    void RemoveHabitat(UHabitatPopulationManager* habitat) { Habitats.Remove(habitat); }
    void AddBuilding(UWorkforceComponent* building);
    void RemoveBuilding(UWorkforceComponent* building);

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( UBiomePopulationManager, STATGROUP_Tickables );
    }
    virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
};
