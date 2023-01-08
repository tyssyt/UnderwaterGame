// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Recipes/Recipe.h"

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
    UTextBlock* Time;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Ingredients;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Results;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UIngredientUI> IngredientUIClass;

    Recipe* RecipeIHateCPP;
    std::function<void(Recipe*)> Callback;

public:
    void SetRecipe(Recipe* recipe, std::function<void(Recipe*)> callback);

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
    void SetRecipes(TArray<Recipe*>* recipes, std::function<void(Recipe*)> callback);

};
