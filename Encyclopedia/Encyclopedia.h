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
    TMap<UClass*, UConstructionPlan*> ClassToBuildings;
    UPROPERTY()
    TMap<UClass*, UNaturalResource*> ClassToNaturalResources;
    
    TMap<UClass*, TArray<URecipe*>> RecipesByBuilding;
    TMap<UResource*, TArray<URecipe*>> RecipesByIngredient;
    TMap<UResource*, TArray<URecipe*>> RecipesByResult;
    TMap<UResource*, TArray<UConstructionPlan*>> BuildingByMaterial;
    TMap<UResource*, TArray<UConstructionPlan*>> BuildingByNeed;
    TMap<UNaturalResource*, TArray<UConstructionPlan*>> BuildingsByNaturalResource;

    TArray<TPair<UResource*, int32>> StartResources; // TODO have this configurable and don't use PickupPads for them

public:

    UEncyclopedia* Init(TMap<FString, UResource*>& resources, TMap<FString, UNaturalResource*>& naturalResources, TMap<FString, UConstructionPlan*>& buildings, const TArray<URecipe*>& recipes);

    // Special Resources
    UPROPERTY()
    UResource* Food;
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

    TArray<UResource*>& GetAllResources();
    TArray<UNaturalResource*>& GetAllNaturalResources();
    TArray<UConstructionPlan*>& GetAllBuildings();
    TArray<URecipe*>& GetAllRecipes();

    UNaturalResource* GetNaturalResource(const UClass* building);
    UConstructionPlan* GetBuilding(const UClass* building);

    TArray<URecipe*>& GetRecipes(UClass* building);
    TArray<URecipe*>& GetRecipesByIngredient(UResource* resource);
    TArray<URecipe*>& GetRecipesByResult(UResource* resource);
    TArray<UConstructionPlan*>& GetBuildingsByMaterial(UResource* resource);
    TArray<UConstructionPlan*>& GetBuildingsByNeed(UResource* resource);
    TArray<UConstructionPlan*>& GetBuildings(UNaturalResource* naturalResource);

    TArray<TPair<UResource*, int32>>& GetStartResources();

    TSet<UResource*> FindConstructionResources();
    TSet<UResource*> FindRawMaterials();
    TSet<UResource*> FindNeeds();

    // TODO some more general mechanism, i.e. Metadata
    CropData* GetCrop(URecipe* recipe);
};
