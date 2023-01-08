// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Recipe.h"
#include "XD/Resources/ResourceBook.h"

#include "Containers/Array.h"
#include "RecipeBook.generated.h"

UCLASS()
class URecipeBook : public UObject {
    GENERATED_BODY()

public:
    URecipeBook* Init(UResourceBook* resourceBook);

    TArray<Recipe*> SmelterRecipes;

};
