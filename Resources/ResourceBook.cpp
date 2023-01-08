// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceBook.h"

UResourceBook::UResourceBook() {

    HardOre = NewObject<UResource>()->Init(FText::FromString(TEXT("Hard Ore")), 1, TEXT("/Game/Assets/Resources/Hard_Ore"));
    LargeParts = NewObject<UResource>()->Init(FText::FromString(TEXT("Large Parts")), 1000, TEXT("/Game/Assets/Resources/Large_parts"));
    SmallParts = NewObject<UResource>()->Init(FText::FromString(TEXT("Small Parts")), 100, TEXT("/Game/Assets/Resources/Small_parts"));

    ConductiveOre = NewObject<UResource>()->Init(FText::FromString(TEXT("Conductive Ore")), 1, TEXT("/Game/Assets/Resources/Conductive_Ore"));
    Cable = NewObject<UResource>()->Init(FText::FromString(TEXT("Cable")), 100, TEXT("/Game/Assets/Resources/Cable"));

    Oil = NewObject<UResource>()->Init(FText::FromString(TEXT("Oil")), 1, TEXT("/Game/Assets/Resources/Oil"));
    Rubber = NewObject<UResource>()->Init(FText::FromString(TEXT("Rubber")), 100, TEXT("/Game/Assets/Resources/Rubber"));

    People = NewObject<UResource>()->Init(FText::FromString(TEXT("People")), 1, TEXT("/Game/Assets/Resources/Peeps"));
    Workforce = NewObject<UResource>()->Init(FText::FromString(TEXT("Workforce")), 1, TEXT("/Game/Assets/Resources/Peeps"));
}
