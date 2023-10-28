// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Recipe.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/WrapBox.h"
#include "XD/Buildings/BuildingUI.h"
#include "XD/Resources/ResourceAmountUI.h"
#include "RecipeUI.generated.h"

UCLASS(Abstract)
class XD_API URecipeUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceAmountUI> ResourceAmountUIClass;
    
public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Ingredients;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    USizeBox* IngredientsBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Results;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    USizeBox* ResultsBox;

    URecipeUI* Init(URecipe* recipe);
};


UCLASS(Abstract)
class XD_API URecipeWithBuildingUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URecipeUI* Recipe;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Buildings;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    USizeBox* BuildingsBox;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBuildingUI> BuildingUIClass;

public:
    URecipeWithBuildingUI* Init(URecipe* recipe);

    static void Layout(const TArray<URecipeWithBuildingUI*>& uis, const double width);
};
