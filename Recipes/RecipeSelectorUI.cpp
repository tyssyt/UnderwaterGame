// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipeSelectorUI.h"

void UIngredientUI::SetIngredient(const Ingredient* ingredient) {
    Count->SetText(FText::AsNumber(ingredient->amount));
    ResourceImage->SetBrushFromTexture(ingredient->resource->Image);
}

void URecipeUI::SetRecipe(URecipe* recipe, const std::function<void(URecipe*)>& callback) {
    RecipeIHateCPP = recipe;
    Callback = callback;

    for (Ingredient const& ingredient : recipe->Ingredients) {
        // TODO is there a way to do this but still have a Pointer?
        UIngredientUI* ingredientUI = CreateWidget<UIngredientUI>(this, IngredientUIClass);
        ingredientUI->SetIngredient(&ingredient);
        Ingredients->AddChildToWrapBox(ingredientUI);
    }
    for (Result const& result : recipe->Results) {
        // TODO is there a way to do this but still have a Pointer?
        UIngredientUI* ingredientUI = CreateWidget<UIngredientUI>(this, IngredientUIClass);
        ingredientUI->SetIngredient(&result);
        Results->AddChildToWrapBox(ingredientUI);
    }
}

void URecipeUI::OnClickRecipeSelect() {
    Callback(RecipeIHateCPP);
}

void URecipeSelectorUI::SetRecipes(const TArray<URecipe*>& recipes, const std::function<void(URecipe*)>& callback) {
    for (URecipe* recipe : recipes) {
        URecipeUI* recipeUI = CreateWidget<URecipeUI>(this, RecipeUIClass);
        recipeUI->SetRecipe(recipe, callback);
        RecipeList->AddChildToVerticalBox(recipeUI);
    }
}
