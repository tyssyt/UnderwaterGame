// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/NaturalResources/NaturalResource.h"
#include "Encyclopedia.generated.h"

struct CropData;
class URecipe;
class UResource;
class UConstructionPlan;
class UNeed;
class UNeedSatisfier;

UCLASS()
class XD_API UEncyclopedia : public UObject {
    GENERATED_BODY()

protected:

    UPROPERTY()
    TArray<UResource*> Resources;
    UPROPERTY()
    TArray<UNaturalResource*> NaturalResources;
    UPROPERTY()
    TArray<UConstructionPlan*> Buildings;
    UPROPERTY()
    TArray<URecipe*> Recipes;
    UPROPERTY()
    TArray<UNeed*> Needs;
    UPROPERTY()
    TArray<UNeedSatisfier*> NeedSatisfiers;

    UPROPERTY()
    TMap<UClass*, UNaturalResource*> ClassToNaturalResources;
    
    TMap<UConstructionPlan*, TArray<URecipe*>> RecipesByBuilding;
    TMap<UResource*, TArray<URecipe*>> RecipesByIngredient;
    TMap<UResource*, TArray<URecipe*>> RecipesByResult;
    TMap<UResource*, TArray<UConstructionPlan*>> BuildingByMaterial;
    TMap<UResource*, TArray<UConstructionPlan*>> BuildingByNeed;
    TMap<UNaturalResource*, TArray<UConstructionPlan*>> BuildingsByNaturalResource;
    TMap<UNeed*, TArray<UNeedSatisfier*>> SatisfierByNeed;
    TMap<UResource*, TArray<UNeedSatisfier*>> SatisfierByGood;
    TMap<UConstructionPlan*, TArray<UNeedSatisfier*>> SatisfierByService;

    TArray<TPair<UResource*, int32>> StartResources; // TODO have this configurable and don't use PickupPads for them

public:

    UEncyclopedia* Init(
        TMap<FString, UResource*>& resources,
        TMap<FString, UNaturalResource*>& naturalResources,
        TMap<FString, UConstructionPlan*>& buildings,
        const TArray<URecipe*>& recipes,
        TMap<FString, UNeed*>& needs,
        const TArray<UNeedSatisfier*>& needSatisfiers
    );

    // Special Resources
    UPROPERTY()
    UResource* People;
    UPROPERTY()
    UResource* Workforce;    

    // Special Buildings
    UPROPERTY()
    UConstructionPlan* Conveyor;
    UPROPERTY()
    UConstructionPlan* ConveyorNode;
    UPROPERTY()
    UConstructionPlan* ConveyorLink;
    UPROPERTY()
    UConstructionPlan* Splitter;
    UPROPERTY()
    UConstructionPlan* Merger;
    UPROPERTY()
    UConstructionPlan* WorkerHouse;

    TArray<UResource*>& GetAllResources() { return Resources; }
    TArray<UNaturalResource*>& GetAllNaturalResources() { return NaturalResources; }
    TArray<UConstructionPlan*>& GetAllBuildings() { return Buildings; }
    TArray<URecipe*>& GetAllRecipes() { return Recipes; }
    TArray<UNeed*>& GetAllNeeds() { return Needs; }

    UNaturalResource* GetNaturalResource(const UClass* building);

    TArray<URecipe*>& GetRecipes(UConstructionPlan* building);
    TArray<URecipe*>& GetRecipesByIngredient(UResource* resource);
    TArray<URecipe*>& GetRecipesByResult(UResource* resource);
    TArray<UConstructionPlan*>& GetBuildingsByMaterial(UResource* resource);
    TArray<UConstructionPlan*>& GetBuildingsByNeed(UResource* resource);
    TArray<UConstructionPlan*>& GetBuildings(UNaturalResource* naturalResource);
    TArray<UNeedSatisfier*>& GetNeedSatisfiers(UNeed* need);
    TArray<UNeedSatisfier*>& GetNeedsByGood(UResource* good);
    TArray<UNeedSatisfier*>& GetNeedsByService(UConstructionPlan* service);

    TArray<TPair<UResource*, int32>>& GetStartResources() { return StartResources; }

    TSet<UResource*> FindConstructionResources();
    TSet<UResource*> FindRawMaterials();
    TSet<UResource*> FindNeeds();
    TSet<UResource*> FindGoods();

    // TODO some more general mechanism, i.e. Metadata
    CropData* GetCrop(URecipe* recipe);
};
