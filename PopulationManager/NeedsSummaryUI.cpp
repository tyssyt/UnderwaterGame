// Fill out your copyright notice in the Description page of Project Settings.


#include "NeedsSummaryUI.h"

#include "HabitatPopulationManager.h"
#include "NeedUI.h"
#include "The.h"
#include "UI.h"
#include "Components/WrapBoxSlot.h"
#include "XD/BlueprintHolder.h"

bool UNeedsSummaryUI::NeedsMatch(int population) {
    const auto encyclopedia = The::Encyclopedia(this);

    int i = 0;    
    for (const auto need : encyclopedia->GetAllNeeds()) {
        if (need->StartPop > population)
            break;

        // TODO we can probably create some util that gets a nested child
        const auto child = Needs->GetChildAt(i);
        if (!child)
            return false;
        
        const auto sizeBox = Cast<USizeBox>(child);
        if (!sizeBox)
            return false;

        const auto border = Cast<UBorder>(sizeBox->GetChildAt(0));
        if (!border)
            return false;

        const auto needUI = Cast<UNeedUI>(border->GetChildAt(0));
        if (!needUI)
            return false;

        if (need != needUI->Need)
            return false;

        i++;
    }

    if (i != Needs->GetChildrenCount())
        return false;

    return true;
}

void UNeedsSummaryUI::InitNeeds(int population) {
    const auto encyclopedia = The::Encyclopedia(this);
    Needs->ClearChildren();
    
    for (const auto need : encyclopedia->GetAllNeeds()) {
        if (need->StartPop > population)
            break;
        
        const auto needUI = CreateWidget<UNeedUI>(this, NeedUIClass)->Init(need);
        const auto border = UX::WithBorder(WidgetTree, needUI, FLinearColor::Transparent);
        const auto sizeBox = UX::Sized(WidgetTree, border, 60., 60.);
        const auto slot = Needs->AddChildToWrapBox(sizeBox);
        //slot->SetHorizontalAlignment(HAlign_Fill);
    }
}

void UNeedsSummaryUI::UpdateSatisfied(const TArray<UNeed*>& satisfiedNeeds) const {
    for (const auto child : Needs->GetAllChildren()) {
        const auto sizeBox = Cast<USizeBox>(child);
        const auto border = Cast<UBorder>(sizeBox->GetChildAt(0));
        const auto needUI = Cast<UNeedUI>(border->GetChildAt(0));

        if (satisfiedNeeds.Contains(needUI->Need))
            border->SetBrushColor(FLinearColor::Transparent);
        else
            border->SetBrushColor(FLinearColor::Red);  
    }
}

void UNeedsSummaryUI::SetNeeds(UHabitatPopulationManager* populationManager) {
    if (!NeedsMatch(populationManager->GetCurrentPop()))
        InitNeeds(populationManager->GetCurrentPop());
    UpdateSatisfied(populationManager->GetSatisfiedNeeds());
}
