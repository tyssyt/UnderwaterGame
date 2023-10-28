// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstanceX.h"

UGameInstanceX::UGameInstanceX() {
    UE_LOG(LogTemp, Warning, TEXT("GameInstanceX Constructor"));
}

void UGameInstanceX::Init() {
    UE_LOG(LogTemp, Warning, TEXT("GameInstanceX Init"));
    TheConstructionManager = NewObject<UConstructionManager>(this);
    TheElectricityManager = NewObject<UElectricityManager>(this);
}
