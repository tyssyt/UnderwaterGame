// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Recipe.h"
#include "RecipeUI.h"
#include "Components/VerticalBox.h"

#include "RecipeSelectorUI.generated.h"

UCLASS(Abstract)
class XD_API URecipeButtonUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URecipeUI* Recipe;

    URecipe* RecipeIHateCPP;
    std::function<void(URecipe*)> Callback;

public:
    URecipeButtonUI* Init(URecipe* recipe, const std::function<void(URecipe*)>& callback);

    UFUNCTION(BlueprintCallable)
    void OnClickRecipeSelect();
};

UCLASS(Abstract)
class XD_API URecipeSelectorUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* RecipeList;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<URecipeButtonUI> RecipeButtonUIClass;

public:
    URecipeSelectorUI* Init(const TArray<URecipe*>& recipes, const std::function<void(URecipe*)>& callback);

    UFUNCTION(BlueprintCallable)
    void OnClose();
};
