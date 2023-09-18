// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceBalanceUI.h"

#include "Resource.h"

void UResourceBalanceUI::SetHave(int have) const {
    Have->SetText(FText::AsNumber(have));

    if (NeedInt > 0 && NeedInt > have)
        Border->SetBrushColor(FLinearColor::Red);
    else
        Border->SetBrushColor(FLinearColor::Transparent);
}

void UResourceBalanceUI::SetNeed(int need, const UResource* resource) {
    NeedInt = need;
    Need->SetText(FText::AsNumber(need));
    ResourceImage->SetBrushFromTexture(resource->Image);    
}
