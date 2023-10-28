// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionUI.h"

void UConstructionUI::Init(UConstructionPlan* constructionPlan, const UConstructionManager* constructionManager) const {
    BuildingName->SetText(constructionPlan->Name);
    Building->Init(constructionPlan);
    TogglePower->SetVisibility(ESlateVisibility::Collapsed);
    ConstructionMaterials->Init(constructionPlan->Materials, constructionManager);
}
