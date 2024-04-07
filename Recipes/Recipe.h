// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD/Resources/Resource.h"

#include "Recipe.generated.h"

class UConstructionPlan;

struct XD_API Ingredient {
    Ingredient(int amount, UResource* resource) : amount(amount), resource(resource) {}

    const int amount;
    UResource* const resource;
};

typedef Ingredient Result;

UCLASS()
class XD_API URecipe : public UObject {
    GENERATED_BODY()

public:
    URecipe* Init(const TArray<UConstructionPlan*>& buildings, const TArray<Ingredient>& ingredients, const TArray<Result>& results);
    URecipe* InitUnchecked(const TArray<UConstructionPlan*>& buildings, const TArray<Ingredient>& ingredients, const TArray<Result>& results);
    
    UPROPERTY(VisibleAnywhere)
    TArray<UConstructionPlan*> Buildings;

    TArray<Ingredient> Ingredients;
    TArray<Result> Results;

    bool HasSize(int ingredients, int results) const;

    bool operator<(const URecipe& other) const;
};
