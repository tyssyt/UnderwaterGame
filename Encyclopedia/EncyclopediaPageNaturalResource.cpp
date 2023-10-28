// Fill out your copyright notice in the Description page of Project Settings.


#include "EncyclopediaPageNaturalResource.h"

#include "Collections.h"
#include "Encyclopedia.h"

void UEncyclopediaPageNaturalResource::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    if (Gives->GetVisibility() != ESlateVisibility::Collapsed)
        return;

    const double width = Gives->GetDesiredSize().X;
    if (width == LastWidth)
        return;
    LastWidth = width;

    URecipeWithBuildingUI::Layout(Arrays::Cast<URecipeWithBuildingUI>(Gives->GetAllChildren()), width);
}

UEncyclopediaPageNaturalResource* UEncyclopediaPageNaturalResource::Init(UNaturalResource* naturalResource, UEncyclopedia* encyclopedia) {
    Description->SetText(naturalResource->Description);

    ExploitedWith->ClearChildren();
    Gives->ClearChildren();
    for (const auto building : encyclopedia->GetBuildings(naturalResource)) {        
        ExploitedWith->AddChildToVerticalBox(
        CreateWidget<UConstructionPlanUI>(GetOwningPlayer(), ConstructionPlanUIClass)->Init(building, true));

        for (const auto recipe : encyclopedia->GetRecipes(building->BuildingClass))
            Gives->AddChildToVerticalBox(
        CreateWidget<URecipeWithBuildingUI>(GetOwningPlayer(), RecipeWithBuildingUIClass)->Init(recipe));
    }
    return this;
}
