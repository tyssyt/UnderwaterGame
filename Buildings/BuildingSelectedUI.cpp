// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingSelectedUI.h"

#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Encyclopedia/Encyclopedia.h"

UBuildingSelectedUI* UBuildingSelectedUI::Init(ABuilding* actor) {
    const auto encyclopedia = The::Encyclopedia(actor);

    Selected = actor;
    Building->Init(actor->GetConstructionPlan());
    Name->SetText(actor->GetConstructionPlan()->Name);

    actor->InitSelectedUI(Components);
    for (const auto component : Components)
        component->CreateUI(this);
    
    if (Top->GetChildrenCount() == 0)
        Top->SetVisibility(ESlateVisibility::Collapsed);
    return this;
}

void UBuildingSelectedUI::Tick() {
    if (!Selected)
        return;

    for (const auto component : Components)
        component->Tick(this);
}
