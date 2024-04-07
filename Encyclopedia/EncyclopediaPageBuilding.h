// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/Border.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "XD/Buildings/ConstructionPlanUI.h"
#include "XD/PopulationManager/NeedSatisfierUI.h"
#include "XD/Recipes/RecipeUI.h"
#include "EncyclopediaPageBuilding.generated.h"

class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageBuilding : public UEncyclopediaPage {
    GENERATED_BODY()

    double LastWidth = 0.;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Description;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* ConstructionPlans;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Needs;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* NeedsBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Gives;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* GivesBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Recipes;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* RecipesBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* SatisfiesNeed;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* SatisfierBox;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConstructionPlanUI> ConstructionPlanUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNaturalResourceUI> NaturalResourceUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceAmountUI> ResourceAmountUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<URecipeWithBuildingUI> RecipeWithBuildingUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNeedSatisfierUI> NeedSatisfierUIClass;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UEncyclopediaPageBuilding* Init(UConstructionPlan* building, UEncyclopedia* encyclopedia);
    UEncyclopediaPageBuilding* InitConveyor(UEncyclopedia* encyclopedia);
};
