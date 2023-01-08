// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipeSelectorUI.h"

void UIngredientUI::SetIngredient(const Ingredient* ingredient) {
    Count->SetText(FText::AsNumber(ingredient->amount));
    ResourceImage->SetBrushFromTexture(ingredient->resource->Image);
}

void URecipeUI::SetRecipe(Recipe* recipe, std::function<void(Recipe*)> callback) {
    RecipeIHateCPP = recipe;
    Callback = callback;

    Time->SetText(FText::Format(FText::FromString(TEXT("{0}s")), FText::AsNumber(recipe->time))); // TODO this does not feel like the "correct" way to do this
    for (Ingredient const& ingredient : recipe->ingredients) {
        // TODO is there a way to do this but still have a Pointer?
        UIngredientUI* ingredientUI = CreateWidget<UIngredientUI>(this, IngredientUIClass);
        ingredientUI->SetIngredient(&ingredient);
        Ingredients->AddChildToWrapBox(ingredientUI);
    }
    for (Result const& result : recipe->results) {
        // TODO is there a way to do this but still have a Pointer?
        UIngredientUI* ingredientUI = CreateWidget<UIngredientUI>(this, IngredientUIClass);
        ingredientUI->SetIngredient(&result);
        Results->AddChildToWrapBox(ingredientUI);
    }
}

void URecipeUI::OnClickRecipeSelect() {
    Callback(RecipeIHateCPP);
}

void URecipeSelectorUI::SetRecipes(TArray<Recipe*>* recipes, std::function<void(Recipe*)> callback) {
    for (Recipe* recipe : *recipes) {
        URecipeUI* recipeUI = CreateWidget<URecipeUI>(this, RecipeUIClass);
        recipeUI->SetRecipe(recipe, callback);
        RecipeList->AddChildToVerticalBox(recipeUI);
    }
}
