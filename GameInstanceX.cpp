// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstanceX.h"

UGameInstanceX::UGameInstanceX() {
    UE_LOG(LogTemp, Warning, TEXT("GameInstanceX Constructor"));
}

void UGameInstanceX::Init() {
    UE_LOG(LogTemp, Warning, TEXT("GameInstanceX Init"));
    TheResourceBook = NewObject<UResourceBook>();
    TheRecipeBook = NewObject<URecipeBook>()->Init(TheResourceBook);
    TheBuildingBook = NewObject<UBuildingBook>()->Init(TheResourceBook);
    TheConstructionManager = NewObject<UConstructionManager>()->Init(TheResourceBook);
}
