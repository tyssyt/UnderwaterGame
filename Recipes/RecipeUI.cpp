// Fill out your copyright notice in the Description page of Project Settings.

#include "RecipeUI.h"

URecipeUI* URecipeUI::Init(URecipe* recipe) {
    Ingredients->ClearChildren();
    for (auto const& ingredient : recipe->Ingredients)
        Ingredients->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(this, ResourceAmountUIClass)->Init(ingredient.amount, ingredient.resource));

    Results->ClearChildren();
    for (auto const& result : recipe->Results)
        Results->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(this, ResourceAmountUIClass)->Init(result.amount, result.resource));
    return this;
}

URecipeWithBuildingUI* URecipeWithBuildingUI::Init(URecipe* recipe) {
    Recipe->Init(recipe);

    Buildings->ClearChildren();
    for (auto const building : recipe->Buildings)
        Buildings->AddChildToWrapBox(CreateWidget<UBuildingUI>(this, BuildingUIClass)->Init(building));
    return this;
}

int divideCeil(const int num, const int by) {
    return (num + by -1) / by;
}

void URecipeWithBuildingUI::Layout(const TArray<URecipeWithBuildingUI*>& uis, const double width) {
    static constexpr double SLOT_WIDTH = 80.;
    static constexpr double SYMBOLS_SIZE = 80.; // combined size of '⇒' & '@'
    if (uis.IsEmpty())
        return;

    const int slots = (width-SYMBOLS_SIZE) / SLOT_WIDTH;
    if (slots < 3)
        return;

    // for each column, get the max number in that column
    int maxIngredients = 0;
    int maxResults = 0;
    int maxBuildings = 0;
    for (const auto ui : uis) {
        maxIngredients = FMath::Max(maxIngredients, ui->Recipe->Ingredients->GetChildrenCount());
        maxResults = FMath::Max(maxResults, ui->Recipe->Results->GetChildrenCount());
        maxBuildings = FMath::Max(maxBuildings, ui->Buildings->GetChildrenCount());
    }

    // calculate how many rows a recipe at most needs
    int requiredRows = 1;
    while (divideCeil(maxIngredients, requiredRows) + divideCeil(maxResults, requiredRows) + divideCeil(maxBuildings, requiredRows) > slots)
        requiredRows++;

    // if every recipe used the full amount of rows, how many slots are "left over"
    const int requiredIngredients = divideCeil(maxIngredients, requiredRows);
    const int requiredResults = divideCeil(maxResults, requiredRows);
    const int requiredBuildings = divideCeil(maxBuildings, requiredRows);

    const int leftoverSlots = slots - (requiredIngredients + requiredResults + requiredBuildings);

    if (requiredRows > 1 && leftoverSlots > 0) {
        // we have some slots that we can freely use
        // TODO optimize such that the amount of rows needed by all uis is minimized
        checkNoEntry();
    }

    // Apply the Layout to all the UIs
    for (const auto ui : uis) {
        ui->Recipe->IngredientsBox->SetWidthOverride(SLOT_WIDTH * requiredIngredients);
        ui->Recipe->ResultsBox->SetWidthOverride(SLOT_WIDTH * requiredResults);
        ui->BuildingsBox->SetWidthOverride(SLOT_WIDTH * requiredBuildings);
    }
}
