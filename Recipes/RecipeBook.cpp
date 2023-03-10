// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipeBook.h"

URecipeBook* URecipeBook::Init(UResourceBook* resourceBook) {
    SmelterRecipes.Add(new Recipe(10, {Ingredient(1000, resourceBook->HardOre)}, Result(1, resourceBook->LargeParts)));
    SmelterRecipes.Add(new Recipe(5, {Ingredient(100, resourceBook->HardOre)}, Result(1, resourceBook->SmallParts)));

    SmelterRecipes.Add(new Recipe(5, {Ingredient(100, resourceBook->ConductiveOre)}, Result(1, resourceBook->Cable)));

    SmelterRecipes.Add(new Recipe(5, {Ingredient(100, resourceBook->Oil)}, Result(1, resourceBook->Rubber)));

    return this;
}
