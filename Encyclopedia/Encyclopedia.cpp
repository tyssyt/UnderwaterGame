// Fill out your copyright notice in the Description page of Project Settings.

#include "Encyclopedia.h"

#include "Collections.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Recipes/Recipe.h"

UEncyclopedia* UEncyclopedia::Init(
    TMap<FString, UResource*>& resources,
    TMap<FString, UNaturalResource*>& naturalResources,
    TMap<FString, UConstructionPlan*>& buildings,
    const TArray<URecipe*>& recipes
) {
    for (const auto& resource : resources)
        Resources.Add(resource.Value);
    for (const auto& naturalResource : naturalResources)
        NaturalResources.Add(naturalResource.Value);
    for (const auto& building : buildings)
        Buildings.Add(building.Value);
    Recipes.Append(recipes);
    
    Resources.Sort();
	NaturalResources.Sort();
    Buildings.Sort();
    Recipes.Sort();

    for (const auto& naturalResource : NaturalResources) {
        ClassToNaturalResources.Add(naturalResource->BuildingClass, naturalResource);
    }
    for (const auto building : Buildings) {
        ClassToBuildings.Add(building->BuildingClass, building);
        for (const auto& material : building->Materials)
            MultiMaps::AddTo(BuildingByMaterial, material.resource, building);
        for (const auto& need : building->GetNeeds())
            MultiMaps::AddTo(BuildingByNeed, need.resource, building);
        if (const auto naturalResource = building->ConstructedOn)
            MultiMaps::AddTo(BuildingsByNaturalResource, naturalResource, building);
    }
    for (const auto& recipe : Recipes) {
        for (const auto building : recipe->Buildings)
            MultiMaps::AddTo(RecipesByBuilding, building->BuildingClass, recipe);
        for (const auto& ingredient : recipe->Ingredients)
            MultiMaps::AddTo(RecipesByIngredient, ingredient.resource, recipe);
        for (const auto& result : recipe->Results)
            MultiMaps::AddTo(RecipesByResult, result.resource, recipe);
    }

    for (auto& entry : BuildingByMaterial)
        entry.Value.Sort(&UConstructionPlan::CompareByComplexity);
    for (auto& entry : BuildingsByNaturalResource)
        entry.Value.Sort(&UConstructionPlan::CompareByComplexity);

    // Special Resources
    // TODO configure start resources in config
    StartResources.Emplace(resources[TEXT("Large Parts")], 3000);
    StartResources.Emplace(resources[TEXT("Mechanical Components")], 1500);
    StartResources.Emplace(resources[TEXT("Electrical Components")], 1500);
    StartResources.Emplace(resources[TEXT("Cable")], 750);
    StartResources.Emplace(resources[TEXT("Rubber")], 750);
    StartResources.Emplace(resources[TEXT("Glass")], 750);
    StartResources.Emplace(resources[TEXT("Life Support Equipment")], 750);
    
    Food = resources[TEXT("Food")];
    People = resources[TEXT("People")];
    Workforce = resources[TEXT("Workforce")];

    // Special Buildings
    Conveyor = buildings[TEXT("Conveyor")];
    ConveyorNode = buildings[TEXT("Conveyor Node")];
    ConveyorLink = buildings[TEXT("Conveyor Link")];
    Splitter = buildings[TEXT("Splitter")];
    Merger = buildings[TEXT("Merger")];
    
    HardOreMine = buildings[TEXT("Hard Ore Mine")];
    ConductiveOreMine = buildings[TEXT("Conductive Ore Mine")];
    OilFishHarvester = buildings[TEXT("Oil Fish Harvester")];    
    
    return this;
}

TArray<UResource*>& UEncyclopedia::GetAllResources() {
    return Resources;
}

TArray<UNaturalResource*>& UEncyclopedia::GetAllNaturalResources() {
    return NaturalResources;
}

TArray<UConstructionPlan*>& UEncyclopedia::GetAllBuildings() {
    return Buildings;
}

TArray<URecipe*>& UEncyclopedia::GetAllRecipes() {
    return Recipes;
}

UNaturalResource* UEncyclopedia::GetNaturalResource(const UClass* building) {
    if (const auto naturalResource = ClassToNaturalResources.Find(building))
        return *naturalResource;
    return nullptr;
}
UConstructionPlan* UEncyclopedia::GetBuilding(const UClass* building) {
    if (const auto constructionPlan = ClassToBuildings.Find(building))
        return *constructionPlan;
    return nullptr;
}

TArray<URecipe*>& UEncyclopedia::GetRecipes(UClass* building) {
    return MultiMaps::Find(RecipesByBuilding, building);
}
TArray<URecipe*>& UEncyclopedia::GetRecipesByIngredient(UResource* resource) {
    return MultiMaps::Find(RecipesByIngredient, resource);
}
TArray<URecipe*>& UEncyclopedia::GetRecipesByResult(UResource* resource) {
    return MultiMaps::Find(RecipesByResult, resource);
}


TArray<UConstructionPlan*>& UEncyclopedia::GetBuildingsByMaterial(UResource* resource) {
    return MultiMaps::Find(BuildingByMaterial, resource);
}
TArray<UConstructionPlan*>& UEncyclopedia::GetBuildingsByNeed(UResource* resource) {
    return MultiMaps::Find(BuildingByNeed, resource);
}
TArray<UConstructionPlan*>& UEncyclopedia::GetBuildings(UNaturalResource* naturalResource) {
    return MultiMaps::Find(BuildingsByNaturalResource, naturalResource);
}

TArray<TPair<UResource*, int32>>& UEncyclopedia::GetStartResources() {
    return StartResources;
}

TSet<UResource*> UEncyclopedia::FindConstructionResources() {
    TSet<UResource*> constructionResources;
    for (const auto building : Buildings)
        for (const auto& material : building->Materials)
            constructionResources.Add(material.resource);
    return MoveTemp(constructionResources);
}

TSet<UResource*> UEncyclopedia::FindRawMaterials() {
    TSet<UResource*> rawMaterials;
    for (const auto recipe : Recipes)
        if (recipe->Ingredients.IsEmpty())
            for (const auto& result : recipe->Results)
                rawMaterials.Add(result.resource);
    return MoveTemp(rawMaterials);
}

TSet<UResource*> UEncyclopedia::FindNeeds() {
    TSet<UResource*> needs;
    for (const auto building : Buildings)
        for (const auto& need : building->GetNeeds())
            needs.Add(need.resource);
    return MoveTemp(needs);
}
