// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Encyclopedia.generated.h"

class URecipe;
class UResource;
class UConstructionPlan;

UCLASS()
class XD_API UEncyclopedia : public UObject {
    GENERATED_BODY()

protected:

    UPROPERTY()
    TArray<UResource*> Resources;
    UPROPERTY()
    TArray<UConstructionPlan*> Buildings;
    UPROPERTY()
    TArray<URecipe*> Recipes;

    TArray<TPair<UResource*, int32>> StartResources; // TODO have this configurable and don't use PickupPads for them
    
    TMap<UClass*, TArray<URecipe*>> RecipesByBuilding;

public:

    UEncyclopedia* Init(TMap<FString, UResource*>& resources, TMap<FString, UConstructionPlan*>& buildings, TArray<URecipe*>& recipes);

    // Special Resources
    UPROPERTY()
    UResource* Food;
    UPROPERTY()
    UResource* Electricity;
    UPROPERTY()
    UResource* People;
    UPROPERTY()
    UResource* Workforce;    

    // Special Buildings
    UPROPERTY()
    UConstructionPlan* ConveyorNode;
    UPROPERTY()
    UConstructionPlan* ConveyorLink;
    UPROPERTY()
    UConstructionPlan* Splitter;
    UPROPERTY()
    UConstructionPlan* Merger;

    // TODO these can be removed from special cases once we configure building upgrades in config
    UPROPERTY()
    UConstructionPlan* HardOreMine;
    UPROPERTY()
    UConstructionPlan* ConductiveOreMine;
    UPROPERTY()
    UConstructionPlan* OilFishHarvester;

    TSet<UResource*> FindConstructionResources();
    TArray<TPair<UResource*, int32>>& GetStartResources();    
    TArray<URecipe*>& GetRecipes(UClass* building);
};
