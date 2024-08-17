// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaPageBuilding.h"

#include "Collections.h"
#include "Encyclopedia.h"

void UEncyclopediaPageBuilding::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (Recipes->GetVisibility() != ESlateVisibility::Collapsed)
        return;

    const double width = Recipes->GetDesiredSize().X;
    if (width == LastWidth)
        return;
    LastWidth = width;

    URecipeWithBuildingUI::Layout(Arrays::Cast<URecipeWithBuildingUI>(Recipes->GetAllChildren()), width);
}

UEncyclopediaPageBuilding* UEncyclopediaPageBuilding::Init(UConstructionPlan* building, UEncyclopedia* encyclopedia) {
    Description->SetText(building->Description);

    ConstructionPlans->ClearChildren();
    ConstructionPlans->AddChildToVerticalBox(
        CreateWidget<UConstructionPlanUI>(GetOwningPlayer(), ConstructionPlanUIClass)->Init(building, false)
    );

    {
        Needs->ClearChildren();

        if (building->ConstructedOn)
            Needs->AddChildToWrapBox(CreateWidget<UNaturalResourceUI>(GetOwningPlayer(), NaturalResourceUIClass)->Init(building->ConstructedOn));

        for (const auto& need : building->GetNeeds())
            if (need.amount > 0)
                Needs->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(GetOwningPlayer(), ResourceAmountUIClass)->Init(need.amount, need.resource));

        if (Needs->GetChildrenCount() == 0)
            NeedsBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    {
        Gives->ClearChildren();

        for (const auto component : building->ComponentLoaders)
            for (const auto& need : component->GetNeeds())
                if (need.amount < 0)
                    Gives->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(GetOwningPlayer(), ResourceAmountUIClass)->Init(-need.amount, need.resource));

        if (Gives->GetChildrenCount() == 0)
            GivesBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    {
        Recipes->ClearChildren();
        for (const auto recipe : encyclopedia->GetRecipes(building))
            Recipes->AddChildToVerticalBox(
            CreateWidget<URecipeWithBuildingUI>(GetOwningPlayer(), RecipeWithBuildingUIClass)->Init(recipe));
        if (Recipes->GetChildrenCount() == 0)
            RecipesBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    {
        SatisfiesNeed->ClearChildren();
        for (const auto satisfier : encyclopedia->GetNeedsByService(building))
            SatisfiesNeed->AddChildToVerticalBox(
                CreateWidget<UNeedSatisfierUI>(GetOwningPlayer(), NeedSatisfierUIClass)->Init(satisfier)
            );
        if (SatisfiesNeed->GetChildrenCount() == 0)
            SatisfierBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    return this;
}

UEncyclopediaPageBuilding* UEncyclopediaPageBuilding::InitConveyor(UEncyclopedia* encyclopedia) {
    Init(encyclopedia->Conveyor, encyclopedia);

    ConstructionPlans->ClearChildren();
    ConstructionPlans->AddChildToVerticalBox(
        CreateWidget<UConstructionPlanUI>(GetOwningPlayer(), ConstructionPlanUIClass)->Init(encyclopedia->ConveyorNode, false)
        );
    ConstructionPlans->AddChildToVerticalBox(
        CreateWidget<UConstructionPlanUI>(GetOwningPlayer(), ConstructionPlanUIClass)->Init(encyclopedia->ConveyorLink, false)
    );
    return this;
}
