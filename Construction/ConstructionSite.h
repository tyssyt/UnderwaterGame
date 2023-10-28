// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Buildings/PickupPad.h"
#include "GameFramework/Actor.h"
#include "ConstructionSite.generated.h"

UCLASS()
class XD_API UConstructionSite : public UObject {
    GENERATED_BODY()

protected:
    TArray<Material> DeliveredMaterial;
    UPROPERTY()
    UConstructionOptions* ConstructionOptions;

public:
    UConstructionSite* Init(AXActor* building, const UConstructionPlan* constructionPlan, UConstructionOptions* options); 
    UConstructionSite* Init(AXActor* building, int time, const TArray<Material>& materials, UConstructionOptions* options);
    
    void SetGhostMaterial(UMaterial* ghostMaterial) const;
    void BeginConstruction();

    UPROPERTY()
    AXActor* Building;
    UPROPERTY()
    int Time;    
    TArray<Material> Materials;
    //class ABuilderShip* BuilderShip;

    TPair<APickupPad*, Material> GetNextDelivery(TArray<struct ConstructionResource>& constructionResources) const;
    void DeliverMaterial(Material material);
};
