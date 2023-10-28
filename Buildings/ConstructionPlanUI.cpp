// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionPlanUI.h"

UConstructionPlanUI* UConstructionPlanUI::Init(UConstructionPlan* constructionPlan, bool includeNeeds) {
    Building->Init(constructionPlan);

    Materials->ClearChildren();

    if (includeNeeds && constructionPlan->ConstructedOn)
        Materials->AddChildToWrapBox(CreateWidget<UNaturalResourceUI>(this, NaturalResourceUIClass)->Init(constructionPlan->ConstructedOn));
    
    for (const auto& material : constructionPlan->Materials)
        Materials->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(this, ResourceAmountUIClass)->Init(material.amount, material.resource));

    if (includeNeeds)
        for (const auto& need : constructionPlan->GetNeeds())
            if (need.amount > 0)
                Materials->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(this, ResourceAmountUIClass)->Init(need.amount, need.resource));

    return this;
}
