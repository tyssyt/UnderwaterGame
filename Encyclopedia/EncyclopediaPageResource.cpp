// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaPageResource.h"

#include "Encyclopedia.h"
#include "UI.h"
#include "Collections.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "XD/Recipes/RecipeUI.h"

void UEncyclopediaPageResource::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
    Super::NativeTick(MyGeometry, InDeltaTime);

    const double width = CreatedBy->GetDesiredSize().X;
    if (width == LastWidth)
        return;
    LastWidth = width;

    URecipeWithBuildingUI::Layout(Arrays::Cast<URecipeWithBuildingUI>(CreatedBy->GetAllChildren()), width);

    if (IngredientIn->GetVisibility() != ESlateVisibility::Collapsed)
        URecipeWithBuildingUI::Layout(Arrays::Cast<URecipeWithBuildingUI>(IngredientIn->GetAllChildren()), width);
}

UEncyclopediaPageResource* UEncyclopediaPageResource::Init(UResource* resource, UEncyclopedia* encyclopedia) {
    Description->SetText(resource->Description);

    {
        CreatedBy->ClearChildren();
        for (const auto recipe : encyclopedia->GetRecipesByResult(resource))
            CreatedBy->AddChildToVerticalBox(
            CreateWidget<URecipeWithBuildingUI>(GetOwningPlayer(), RecipeWithBuildingUIClass)->Init(recipe));
    }

    {
        IngredientIn->ClearChildren();
        for (const auto recipe : encyclopedia->GetRecipesByIngredient(resource))
            IngredientIn->AddChildToVerticalBox(
            CreateWidget<URecipeWithBuildingUI>(GetOwningPlayer(), RecipeWithBuildingUIClass)->Init(recipe));
        if (IngredientIn->GetChildrenCount() == 0)
            IngredientBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    {
        NeededBy->ClearChildren();
        for (const auto building : encyclopedia->GetBuildingsByNeed(resource)) {
            bool isPos = true;
            for (const auto need : building->GetNeeds()) {
                if (need.resource == resource) {
                    isPos = need.amount >= 0;
                    break;
                }
            }

            const auto needsBox = WidgetTree->ConstructWidget<UHorizontalBox>();
            for (const auto need : building->GetNeeds()) {
                if (isPos && need.amount > 0) {
                    const auto needWidget = CreateWidget<UResourceAmountUI>(GetOwningPlayer(), ResourceAmountUIClass)->Init(need.amount, need.resource);
                    needsBox->AddChildToHorizontalBox(needWidget);
                } else if (!isPos && need.amount < 0) {
                    const auto needWidget = CreateWidget<UResourceAmountUI>(GetOwningPlayer(), ResourceAmountUIClass)->Init(-need.amount, need.resource);
                    needsBox->AddChildToHorizontalBox(needWidget);
                }
            }

            // TODO experiment to construct UI without WBP
            const auto outerBox = WidgetTree->ConstructWidget<UHorizontalBox>();
            const auto arrowText = WidgetTree->ConstructWidget<UTextBlock>();
            arrowText->SetText(FText::FromString(TEXT("⇒")));
            arrowText->SetFont(UX::WithSize(arrowText->GetFont(), 30));
            const auto buildingUI = CreateWidget<UBuildingUI>(GetOwningPlayer(), BuildingUIClass)->Init(building);
            if (isPos) {
                outerBox->AddChildToHorizontalBox(needsBox);
                outerBox->AddChildToHorizontalBox(arrowText)->SetVerticalAlignment(VAlign_Center);
                outerBox->AddChildToHorizontalBox(buildingUI); 
                NeededBy->AddChildToVerticalBox(outerBox);
            } else {                     
                outerBox->AddChildToHorizontalBox(buildingUI); 
                outerBox->AddChildToHorizontalBox(arrowText)->SetVerticalAlignment(VAlign_Center);
                outerBox->AddChildToHorizontalBox(needsBox);
                CreatedBy->AddChildToVerticalBox(outerBox);
            }
        }

        if (NeededBy->GetChildrenCount() == 0)
            NeedBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    {
        MaterialFor->ClearChildren();
        for (const auto building : encyclopedia->GetBuildingsByMaterial(resource))
            MaterialFor->AddChildToVerticalBox(
                CreateWidget<UConstructionPlanUI>(GetOwningPlayer(), ConstructionPlanUIClass)->Init(building, true)
            );
        if (MaterialFor->GetChildrenCount() == 0)
            MaterialBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    {
        SatisfiesNeed->ClearChildren();
        for (const auto satisfier : encyclopedia->GetNeedsByGood(resource))
            SatisfiesNeed->AddChildToVerticalBox(
                CreateWidget<UNeedSatisfierUI>(GetOwningPlayer(), NeedSatisfierUIClass)->Init(satisfier)
            );
        if (SatisfiesNeed->GetChildrenCount() == 0)
            SatisfierBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    return this;
}
