// Fill out your copyright notice in the Description page of Project Settings.

#include "Recipe.h"

#include "XD/Inventory/InventoryComponent.h"


Ingredient::Ingredient(int amount, const UResource* resource) : amount(amount), resource(resource) {}
Ingredient::~Ingredient() {}

URecipe* URecipe::Init(const TArray<UConstructionPlan*>& buildings, const TArray<Ingredient>& ingredients, const TArray<Result>& results) {
    if (buildings.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("Recipe must have at least one Building"));
        return nullptr;
    }
    if (ingredients.Num() == 0 && results.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("Recipe must have at least one Ingredient or Result"));
        return nullptr;
    }
    
    Buildings.Append(buildings);
    Ingredients.Append(ingredients);
    Results.Append(results);
    return this;
}

bool URecipe::HasSize(int ingredients, int results) const {
    return ingredients == Ingredients.Num() && results == Results.Num();
}
