// Fill out your copyright notice in the Description page of Project Settings.

#include "Encyclopedia.h"

#include "Collections.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Buildings/Farm.h"
#include "XD/PopulationManager/NeedSatisfier.h"
#include "XD/Recipes/Recipe.h"

UEncyclopedia* UEncyclopedia::Init(
    TMap<FString, UResource*>& resources,
    TMap<FString, UNaturalResource*>& naturalResources,
    TMap<FString, UConstructionPlan*>& buildings,
    const TArray<URecipe*>& recipes,
    TMap<FString, UNeed*>& needs,
    const TArray<UNeedSatisfier*>& needSatisfiers
) {
    for (const auto& resource : resources)
        Resources.Add(resource.Value);
    for (const auto& naturalResource : naturalResources)
        NaturalResources.Add(naturalResource.Value);
    for (const auto& building : buildings)
        Buildings.Add(building.Value);
    Recipes.Append(recipes);

    for (const auto& need : needs)
        Needs.Add(need.Value);
    NeedSatisfiers.Append(needSatisfiers);
    
    Resources.Sort();
	NaturalResources.Sort();
    Buildings.Sort();
    Recipes.Sort();
    Needs.Sort();

    for (const auto& naturalResource : NaturalResources) {
        ClassToNaturalResources.Add(naturalResource->BuildingClass, naturalResource);
    }
    for (const auto building : Buildings) {
        for (const auto& material : building->Materials)
            MultiMaps::AddTo(BuildingByMaterial, material.resource, building);
        for (const auto& need : building->GetNeeds())
            MultiMaps::AddTo(BuildingByNeed, need.resource, building);
        if (const auto naturalResource = building->ConstructedOn)
            MultiMaps::AddTo(BuildingsByNaturalResource, naturalResource, building);
    }
    for (const auto& recipe : Recipes) {
        for (const auto building : recipe->Buildings)
            MultiMaps::AddTo(RecipesByBuilding, building, recipe);
        for (const auto& ingredient : recipe->Ingredients)
            MultiMaps::AddTo(RecipesByIngredient, ingredient.resource, recipe);
        for (const auto& result : recipe->Results)
            MultiMaps::AddTo(RecipesByResult, result.resource, recipe);
    }
    for (const auto& satisfier : NeedSatisfiers) {
        MultiMaps::AddTo(SatisfierByNeed, satisfier->Need, satisfier);
        for (const auto& good : satisfier->Goods)
            MultiMaps::AddTo(SatisfierByGood, good.Resource, satisfier);
        for (const auto service : satisfier->Services)
            MultiMaps::AddTo(SatisfierByService, service, satisfier);
    }

    for (auto& entry : BuildingByMaterial)
        entry.Value.Sort(&UConstructionPlan::CompareByComplexity);
    for (auto& entry : BuildingsByNaturalResource)
        entry.Value.Sort(&UConstructionPlan::CompareByComplexity);

    // Special Resources
    // TODO configure start resources in config
    StartResources.Emplace(resources[TEXT("Large Parts")], 500);
    StartResources.Emplace(resources[TEXT("Mechanical Components")], 800);
    StartResources.Emplace(resources[TEXT("Electrical Components")], 2600);
    StartResources.Emplace(resources[TEXT("Cable")], 800);
    StartResources.Emplace(resources[TEXT("Rubber")], 100);
    StartResources.Emplace(resources[TEXT("Glass")], 1500);
    StartResources.Emplace(resources[TEXT("Life Support Equipment")], 850);
    
    People = resources[TEXT("People")];
    Workforce = resources[TEXT("Workforce")];

    // Special Buildings
    Conveyor = buildings[TEXT("Conveyor")];
    ConveyorNode = buildings[TEXT("Conveyor Node")];
    ConveyorLink = buildings[TEXT("Conveyor Link")];
    Splitter = buildings[TEXT("Splitter")];
    Merger = buildings[TEXT("Merger")];
    WorkerHouse = buildings[TEXT("Worker House")];
    
    return this;
}

UNaturalResource* UEncyclopedia::GetNaturalResource(const UClass* building) {
    if (const auto naturalResource = ClassToNaturalResources.Find(building))
        return *naturalResource;
    return nullptr;
}

TArray<URecipe*>& UEncyclopedia::GetRecipes(UConstructionPlan* building) {
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

TArray<UNeedSatisfier*>& UEncyclopedia::GetNeedSatisfiers(UNeed* need) {
    return MultiMaps::Find(SatisfierByNeed, need);
}
TArray<UNeedSatisfier*>& UEncyclopedia::GetNeedsByGood(UResource* good) {
    return MultiMaps::Find(SatisfierByGood, good);
}
TArray<UNeedSatisfier*>& UEncyclopedia::GetNeedsByService(UConstructionPlan* service) {
    return MultiMaps::Find(SatisfierByService, service);
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

TSet<UResource*> UEncyclopedia::FindGoods() {
    TSet<UResource*> goods;
    for (const auto satisfier : NeedSatisfiers)
        for (const auto& good : satisfier->Goods)
            goods.Add(good.Resource);
    return MoveTemp(goods);
}

CropData* UEncyclopedia::GetCrop(URecipe* recipe) {
    // TODO store this in config
    static const auto FOOD_CROP = new CropData( // TODO what happens if I don't make this a pointer?
        {
            TEXT("/Game/Megascans/3D_Plants/Goldenrods_xg1lbfxia/S_Goldenrods_xg1lbfxia_Var2_lod0"),
            TEXT("/Game/Megascans/3D_Plants/Goldenrods_xg1lbfxia/S_Goldenrods_xg1lbfxia_Var4_lod0"),
        },
        7,
        16,
        FVector(1. ,1., 0.),
        FVector(0.18),
        FVector(0.03)
        );
    static const auto ALGAE_CROP =  new CropData(
        {
            TEXT("/Game/Megascans/3D_Plants/Janet_Craig_Dracaena_siBl5/S_Janet_Craig_Dracaena_siBl5_Var1_lod0"),
            TEXT("/Game/Megascans/3D_Plants/Janet_Craig_Dracaena_siBl5/S_Janet_Craig_Dracaena_siBl5_Var2_lod0"),
            TEXT("/Game/Megascans/3D_Plants/Janet_Craig_Dracaena_siBl5/S_Janet_Craig_Dracaena_siBl5_Var4_lod0"),
            TEXT("/Game/Megascans/3D_Plants/Janet_Craig_Dracaena_siBl5/S_Janet_Craig_Dracaena_siBl5_Var5_lod0"),
        },
        7,
        14,
        FVector(1. ,1., 0.),
        FVector(0.15),
        FVector(0.03)
    );
    // Periwinkle is also nice for a crop

    const auto name = recipe->Results[0].resource->Name;
    if (name.EqualTo(FText::FromString(TEXT("Food"))))
        return FOOD_CROP;
    if (name.EqualTo(FText::FromString(TEXT("Algae"))))
        return ALGAE_CROP;

    checkNoEntry();
    return FOOD_CROP;
}
