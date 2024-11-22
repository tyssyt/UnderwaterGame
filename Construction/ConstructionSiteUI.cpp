// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSiteUI.h"

#include "ConstructionSite.h"
#include "UI.h"

UConstructionSiteUI* UConstructionSiteUI::Init(UConstructionSite* constructionSite) {
    ConstructionSite = constructionSite;
    const auto constructionPlan = constructionSite->Building->GetConstructionPlan();
    Building->Init(constructionPlan);
    BuildingName->SetText(constructionPlan->Name);

    Materials->ClearChildren();
    for (const auto& mat : constructionSite->Materials) {        
        const auto ui = CreateWidget<UResourceBalanceUI>(this, ResourceBalanceUIClass);
        ui->SetNeed(mat.amount, mat.resource);
        Materials->AddChild(UX::Sized(this->WidgetTree, ui, 60.f, 60.f));
    }

    Tick();
    return this;
}

void UConstructionSiteUI::Tick() {
    int i=0;
    
    for (const auto& mat : ConstructionSite->Materials) {
        const auto resourceBalanceUi = Cast<UResourceBalanceUI>(Cast<USizeBox>(Materials->GetChildAt(i))->GetChildAt(0));
        resourceBalanceUi->SetHave(ConstructionSite->IsDelivered(mat.resource) ? mat.amount : 0);
        i++;
    }

    Progress->SetPercent(static_cast<float>(ConstructionSite->Progress)/ConstructionSite->Time);
    int timeRemaining = ConstructionSite->Time - ConstructionSite->Progress;
    TimeRemaining->SetText(FText::FromString(FString::Printf(TEXT("%ds"), FMath::DivideAndRoundUp(timeRemaining, 60))));
}
