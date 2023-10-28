// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderMode.h"

#include "The.h"
#include "XD/BlueprintHolder.h"

void UBuilderMode::PreInit() {
    ConstructionUI = CreateWidget<UConstructionUI>(The::PlayerController(this), The::BPHolder(this)->ConstructionUIClass);
}
