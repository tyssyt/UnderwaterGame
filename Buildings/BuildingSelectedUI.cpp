// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingSelectedUI.h"

#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Encyclopedia/Encyclopedia.h"

USelectedUIData* USelectedUIStorage::Get(const UClass* Class) const {
    if (const auto data = Data.Find(Class))
        return *data;
    return nullptr;
}

UBuildingSelectedUI* UBuildingSelectedUI::Init(ABuilding* actor) {
    const auto encyclopedia = The::Encyclopedia(actor);

    const auto constructionPlan = encyclopedia->GetBuilding(actor->GetClass());
    check(constructionPlan);
    if (!constructionPlan)
        return nullptr;

    Selected = actor;
    Building->Init(constructionPlan);
    Name->SetText(constructionPlan->Name);
    Storage = NewObject<USelectedUIStorage>();

    actor->InitSelectedUI(this);
    if (Top->GetChildrenCount() == 0)
        Top->SetVisibility(ESlateVisibility::Collapsed);
    return this;
}

void UBuildingSelectedUI::Tick() {
    if (Selected)
        Selected->UpdateSelectedUI(this);
}
