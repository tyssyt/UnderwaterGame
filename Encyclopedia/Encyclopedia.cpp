// Fill out your copyright notice in the Description page of Project Settings.


#include "Encyclopedia.h"

#include "XD/Utils.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Recipes/Recipe.h"

UEncyclopedia* UEncyclopedia::Init(
    TMap<FString, UResource*>& resources,
    TMap<FString, UConstructionPlan*>& buildings,
    TArray<URecipe*>& recipes
) {

    for (const auto& resource : resources)
        Resources.Add(resource.Value);
    for (const auto& building : buildings)
        Buildings.Add(building.Value);
    Recipes.Append(recipes);

    for (const auto& recipe : Recipes)
        for (const auto building : recipe->Buildings)
            MultiMaps::AddTo(RecipesByBuilding, building->BuildingClass, recipe);

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
    Electricity = resources[TEXT("Electricity")];
    People = resources[TEXT("People")];
    Workforce = resources[TEXT("Workforce")];

    // Special Buildings
    ConveyorNode = buildings[TEXT("Conveyor Node")];
    ConveyorLink = buildings[TEXT("Conveyor Link")];
    Splitter = buildings[TEXT("Splitter")];
    Merger = buildings[TEXT("Merger")];
    
    HardOreMine = buildings[TEXT("Hard Ore Mine")];
    ConductiveOreMine = buildings[TEXT("Conductive Ore Mine")];
    OilFishHarvester = buildings[TEXT("Oil Fish Harvester")];    
    
    return this;
}

TSet<UResource*> UEncyclopedia::FindConstructionResources() {
    TSet<UResource*> constructionResources;
    for (const auto& building : Buildings)
        for (const auto& material : building->Materials)
            constructionResources.Add(material.resource);
    return MoveTemp(constructionResources);
}

TArray<TPair<UResource*, int32>>& UEncyclopedia::GetStartResources() {
    return StartResources;
}

TArray<URecipe*>& UEncyclopedia::GetRecipes(UClass* building) {
    return MultiMaps::Find(RecipesByBuilding, building);
}
