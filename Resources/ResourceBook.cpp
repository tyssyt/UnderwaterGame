// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceBook.h"

UResourceBook::UResourceBook() {
    HardOre = CreateDefaultSubobject<UResource>(TEXT("Hard Ore"))->Init(FText::FromString(TEXT("Hard Ore")), 1, TEXT("/Game/Assets/Resources/Hard_Ore"));
    LargeParts = CreateDefaultSubobject<UResource>(TEXT("Large Parts"))->Init(FText::FromString(TEXT("Large Parts")), 1000, TEXT("/Game/Assets/Resources/Large_parts"));
    SmallParts = CreateDefaultSubobject<UResource>(TEXT("Small Parts"))->Init(FText::FromString(TEXT("Small Parts")), 100, TEXT("/Game/Assets/Resources/Small_parts"));

    ConductiveOre = CreateDefaultSubobject<UResource>(TEXT("Conductive Ore"))->Init(FText::FromString(TEXT("Conductive Ore")), 1, TEXT("/Game/Assets/Resources/Conductive_Ore"));
    ConductiveParts = CreateDefaultSubobject<UResource>(TEXT("Conductive parts"))->Init(FText::FromString(TEXT("Conductive parts")), 100, TEXT("/Game/Assets/Resources/Conductive_Parts"));

    Oil = CreateDefaultSubobject<UResource>(TEXT("Oil"))->Init(FText::FromString(TEXT("Oil")), 1, TEXT("/Game/Assets/Resources/Oil"));
    Rubber = CreateDefaultSubobject<UResource>(TEXT("Rubber"))->Init(FText::FromString(TEXT("Rubber")), 100, TEXT("/Game/Assets/Resources/Rubber"));
    
    MechanicalComponents = CreateDefaultSubobject<UResource>(TEXT("Mechanical Components"))->Init(FText::FromString(TEXT("Mechanical Components")), 1, TEXT("/Game/Assets/Resources/Mechanical_Components"));
    ElectricalComponents = CreateDefaultSubobject<UResource>(TEXT("Electrical Components"))->Init(FText::FromString(TEXT("Electrical Components")), 1, TEXT("/Game/Assets/Resources/Electrical_Components"));
    Cable = CreateDefaultSubobject<UResource>(TEXT("Cable"))->Init(FText::FromString(TEXT("Cable")), 1, TEXT("/Game/Assets/Resources/Cable"));    

    People = CreateDefaultSubobject<UResource>(TEXT("People"))->Init(FText::FromString(TEXT("People")), 1, TEXT("/Game/Assets/Resources/Peeps"));
    Workforce = CreateDefaultSubobject<UResource>(TEXT("Workforce"))->Init(FText::FromString(TEXT("Workforce")), 1, TEXT("/Game/Assets/Resources/Peeps"));
}
