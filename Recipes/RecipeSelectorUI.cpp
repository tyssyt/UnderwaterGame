// Fill out your copyright notice in the Description page of Project Settings.

#include "RecipeSelectorUI.h"

URecipeButtonUI* URecipeButtonUI::Init(URecipe* recipe, const std::function<void(URecipe*)>& callback) {
    RecipeIHateCPP = recipe;
    Callback = callback;
    Recipe->Init(recipe);
    return this;
}

void URecipeButtonUI::OnClickRecipeSelect() {
    Callback(RecipeIHateCPP);
}

URecipeSelectorUI* URecipeSelectorUI::Init(const TArray<URecipe*>& recipes, const std::function<void(URecipe*)>& callback) {
    RecipeList->ClearChildren();
    for (const auto recipe : recipes) {
        const auto recipeUI = CreateWidget<URecipeButtonUI>(this, RecipeButtonUIClass)->Init(recipe, callback);
        RecipeList->AddChildToVerticalBox(recipeUI);
    }
    GetOwningPlayer()->InputComponent->BindAction("Deselect", IE_Pressed, this, &URecipeSelectorUI::OnClose);
    return this;
}

void URecipeSelectorUI::OnClose() {
    RemoveFromParent();
    GetOwningPlayer()->InputComponent->RemoveActionBinding("Deselect", IE_Pressed);
}
