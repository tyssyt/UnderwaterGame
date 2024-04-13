// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderModeExtension.h"

#include "XD/PlayerControllerX.h"

void USubstationBuilderModeExtension::Init(ABuilding* preview, UConstructionUI* constructionUI) {
    check(preview->IsA(ASubstation::StaticClass()));
    Preview = Cast<ASubstation>(preview);
    ConstructionUI = constructionUI;

    constructionUI->TogglePower->SetVisibility(ESlateVisibility::Visible);
}
