// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstanceX.h"

UGameInstanceX::UGameInstanceX() {
    UE_LOG(LogTemp, Warning, TEXT("GameInstanceX Constructor"));
}

void UGameInstanceX::Init() {
    UE_LOG(LogTemp, Warning, TEXT("GameInstanceX Init"));
    TheResourceBook = NewObject<UResourceBook>(this);
    TheRecipeBook = NewObject<URecipeBook>(this)->Init(TheResourceBook);
    TheBuildingBook = NewObject<UBuildingBook>(this)->Init(TheResourceBook);
    TheConstructionManager = NewObject<UConstructionManager>(this)->Init(TheResourceBook);
    TheElectricityManager = NewObject<UElectricityManager>(this);
}
