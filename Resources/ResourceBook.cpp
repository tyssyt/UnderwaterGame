// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceBook.h"

UResource* UResourceBook::MakeResource(FName name, int relativeInventorySize, const TCHAR* image, bool constructionResource) {
    const auto res = CreateDefaultSubobject<UResource>(name)->Init(FText::FromName(name), relativeInventorySize, image, constructionResource);
    All.Add(res);
    return res;
}

UResourceBook::UResourceBook() {
    HardOre = MakeResource(TEXT("Hard Ore"), 1, TEXT("/Game/Assets/Resources/Hard_Ore"), false);
    LargeParts = MakeResource(TEXT("Large Parts"), 1000, TEXT("/Game/Assets/Resources/Large_parts"), true);
    SmallParts = MakeResource(TEXT("Small Parts"), 100, TEXT("/Game/Assets/Resources/Small_parts"), false);

    ConductiveOre = MakeResource(TEXT("Conductive Ore"), 1, TEXT("/Game/Assets/Resources/Conductive_Ore"), false);
    ConductiveParts = MakeResource(TEXT("Conductive parts"), 100, TEXT("/Game/Assets/Resources/Conductive_Parts"), false);

    Oil = MakeResource(TEXT("Oil"), 1, TEXT("/Game/Assets/Resources/Oil"), false);
    Rubber = MakeResource(TEXT("Rubber"), 100, TEXT("/Game/Assets/Resources/Rubber"), true);

    Sand = MakeResource(TEXT("Sand"), 1, TEXT("/Game/Assets/Resources/Sand"), false);
    Glass = MakeResource(TEXT("Glass"), 100, TEXT("/Game/Assets/Resources/Glass"), true);
    
    MechanicalComponents = MakeResource(TEXT("Mechanical Components"), 100, TEXT("/Game/Assets/Resources/Mechanical_Components"), true);
    ElectricalComponents = MakeResource(TEXT("Electrical Components"), 100, TEXT("/Game/Assets/Resources/Electrical_Components"), true);
    Cable = MakeResource(TEXT("Cable"), 100, TEXT("/Game/Assets/Resources/Cable"), true);
    LifeSupportEquipment = MakeResource(TEXT("Life Support Equipment"), 100, TEXT("/Game/Assets/Resources/Placeholder"), true); // TODO Image

    People = MakeResource(TEXT("People"), 1, TEXT("/Game/Assets/Resources/Peeps"), false);
    Workforce = MakeResource(TEXT("Workforce"), 1, TEXT("/Game/Assets/Resources/Peeps"), false);
    Electricity = MakeResource(TEXT("Electricity"), 1, TEXT("/Game/Assets/Resources/Electricity"), false);
}
