// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Recipe.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Containers/Array.h"
#include "Components/WrapBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include <functional>

#include "RecipeSelectorUI.generated.h"

UCLASS(Abstract)
class XD_API UIngredientUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Count;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    void SetIngredient(const Ingredient* ingredient);

};

UCLASS(Abstract)
class XD_API URecipeUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Ingredients;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Results;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UIngredientUI> IngredientUIClass;

    URecipe* RecipeIHateCPP;
    std::function<void(URecipe*)> Callback;

public:
    void SetRecipe(URecipe* recipe, const std::function<void(URecipe*)>& callback);

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
    TSubclassOf<URecipeUI> RecipeUIClass;

public:
    void SetRecipes(const TArray<URecipe*>& recipes, const std::function<void(URecipe*)>& callback);

};
