// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaPageNeed.h"

#include "Encyclopedia.h"

UEncyclopediaPageNeed* UEncyclopediaPageNeed::Init(UNeed* need, UEncyclopedia* encyclopedia) {
    Description->SetText(need->Description);
    InterestStart->Init(need->StartPop, encyclopedia->People);
    FullNeed->Init(need->FullPop, encyclopedia->People);

    {
        SatisfiedBy->ClearChildren();
        for (const auto satisfier : encyclopedia->GetNeedSatisfiers(need))
            SatisfiedBy->AddChildToVerticalBox(
                CreateWidget<UNeedSatisfierUI>(GetOwningPlayer(), NeedSatisfierUIClass)->Init(satisfier)
            );
        if (SatisfiedBy->GetChildrenCount() == 0)
            SatisfierBox->SetVisibility(ESlateVisibility::Collapsed);
    }

    return this;
}
