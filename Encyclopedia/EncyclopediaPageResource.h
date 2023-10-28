// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/Border.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "XD/Buildings/ConstructionPlanUI.h"
#include "EncyclopediaPageResource.generated.h"

class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageResource : public UEncyclopediaPage {
    GENERATED_BODY()

    double LastWidth = 0.;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Description;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* CreatedBy;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* IngredientIn;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* IngredientBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* NeededBy;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* NeedBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* MaterialFor;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBorder* MaterialBox;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<URecipeWithBuildingUI> RecipeWithBuildingUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConstructionPlanUI> ConstructionPlanUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceAmountUI> ResourceAmountUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBuildingUI> BuildingUIClass;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UEncyclopediaPageResource* Init(UResource* resource, UEncyclopedia* encyclopedia);
};
