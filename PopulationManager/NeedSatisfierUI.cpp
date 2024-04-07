// Fill out your copyright notice in the Description page of Project Settings.

#include "NeedSatisfierUI.h"

#include "NeedSatisfier.h"
#include "The.h"

UNeedSatisfierUI* UNeedSatisfierUI::Init(UNeedSatisfier* satisfier) {
    const auto encyclopedia = The::Encyclopedia(this);

    Satisfiers->ClearChildren();
    check(satisfier->Goods.Num() + satisfier->Services.Num() == 1); // TODO make a UI for more complex need satisfiers
    for (const auto& good : satisfier->Goods)
        Satisfiers->AddChildToHorizontalBox(CreateWidget<UResourceRatioUI>(this, ResourceRatioUIClass)->Init(1, good.Resource, good.PeoplePerResource, encyclopedia->People));
    for (const auto service : satisfier->Services)
        Satisfiers->AddChildToHorizontalBox(CreateWidget<UBuildingUI>(this, BuildingUIClass)->Init(service));

    Need->Init(satisfier->Need);
    return this;
}
