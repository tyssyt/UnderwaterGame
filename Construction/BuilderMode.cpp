// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderMode.h"

#include "The.h"
#include "XD/BlueprintHolder.h"

UInBuilderMode::UInBuilderMode() {
    Type = EType::NonInteractable;
}

void UBuilderMode::PreInit() {
    ConstructionUI = CreateWidget<UConstructionUI>(The::PlayerController(this), The::BPHolder(this)->ConstructionUIClass);

    HighlightedValid = NewObject<UHighlighted>()->WithSource(this)->SetColor(UHighlighted::Green);
    HighlightedInvalid = NewObject<UHighlighted>()->WithSource(this)->SetColor(UHighlighted::Red);
    HighlightedOption = NewObject<UHighlighted>()->WithSource(this)->SetColor(UHighlighted::Yellow);
}
