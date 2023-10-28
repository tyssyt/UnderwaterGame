// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerUI.h"

void UPowerUI::Set(int production, int consumption) const {
    Production->SetText(FText::AsNumber(production, &FNumberFormattingOptions::DefaultNoGrouping()));
    Consumption->SetText(FText::AsNumber(consumption, &FNumberFormattingOptions::DefaultNoGrouping()));

    const int balance = production - consumption;
    Balance->SetText(FText::AsNumber(balance, &FNumberFormattingOptions::DefaultNoGrouping()));
    if (balance < 0)
        Balance->SetColorAndOpacity(GetConsumptionColor());
    else
        Balance->SetColorAndOpacity(GetProductionColor());
}

FSlateColor UPowerUI::GetProductionColor() const {
    return Production->GetColorAndOpacity();
}
FSlateColor UPowerUI::GetConsumptionColor() const {
    return Consumption->GetColorAndOpacity();
}
