// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceAmountUI.h"

UResourceAmountUI* UResourceAmountUI::Init(int amount, UResource* resource) {
    Count->SetText(FText::AsNumber(amount, &FNumberFormattingOptions::DefaultNoGrouping()));
    Resource->Init(resource);
    SetToolTipText(resource->Name);
    return this;
}
