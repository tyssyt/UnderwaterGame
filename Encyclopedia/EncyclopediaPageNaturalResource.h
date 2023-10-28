// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "XD/Buildings/ConstructionPlanUI.h"
#include "XD/Recipes/RecipeUI.h"
#include "EncyclopediaPageNaturalResource.generated.h"

class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageNaturalResource : public UEncyclopediaPage {
    GENERATED_BODY()

    double LastWidth = 0.;

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    URichTextBlock* Description;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* ExploitedWith;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Gives;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<URecipeWithBuildingUI> RecipeWithBuildingUIClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UConstructionPlanUI> ConstructionPlanUIClass;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UEncyclopediaPageNaturalResource* Init(UNaturalResource* naturalResource, UEncyclopedia* encyclopedia);
};
