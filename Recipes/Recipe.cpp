// Fill out your copyright notice in the Description page of Project Settings.

#include "Recipe.h"

#include "ProductionComponent.h"
#include "XD/Buildings/ConstructionPlan.h"

Ingredient::Ingredient(int amount, UResource* resource) : amount(amount), resource(resource) {}
Ingredient::~Ingredient() {}

bool HasProductionComponent(UConstructionPlan* building) {
    for (const auto component : building->ComponentLoaders)
        if (component->ComponentInfo->ComponentClass == UProductionComponent::StaticClass())
            return true;
    return false;
}

URecipe* URecipe::Init(const TArray<UConstructionPlan*>& buildings, const TArray<Ingredient>& ingredients, const TArray<Result>& results) {
    if (buildings.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("Recipe must have at least one Building"));
        return nullptr;
    }
    if (ingredients.Num() == 0 && results.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("Recipe must have at least one Ingredient or Result"));
        return nullptr;
    }
    for (const auto building : buildings)
        check(HasProductionComponent(building));

    Buildings.Append(buildings);
    Ingredients.Append(ingredients);
    Results.Append(results);
    return this;
}

URecipe* URecipe::InitUnchecked(const TArray<UConstructionPlan*>& buildings, const TArray<Ingredient>& ingredients, const TArray<Result>& results) {
    Buildings.Append(buildings);
    Ingredients.Append(ingredients);
    Results.Append(results);
    return this;
}

bool URecipe::HasSize(int ingredients, int results) const {
    return ingredients == Ingredients.Num() && results == Results.Num();
}

bool URecipe::operator<(const URecipe& other) const {
    // first by building
    const auto c1 = Buildings[0]->Name.CompareTo(other.Buildings[0]->Name);
    if (c1 != 0)
        return c1 < 0;

    // then by number of ingredients
    const auto c2 = Ingredients.Num() - other.Ingredients.Num();
    if (c2 != 0)
        return c2 < 0;

    // then by number of results
    const auto c3 = Results.Num() - other.Results.Num();
    if (c3 != 0)
        return c3 < 0;
    
    // then by result name
    for (int idx = 0; idx < Results.Num(); idx++) {        
        const auto c4 = Results[idx].resource->Name.CompareTo(other.Results[idx].resource->Name);
        if (c4 != 0)
            return c4 < 0;
    }

    checkNoEntry();
    return this - &other < 0;
}
