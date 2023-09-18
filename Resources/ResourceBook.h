// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Resource.h"
#include "ResourceBook.generated.h"

UCLASS()
class UResourceBook : public UObject {
    GENERATED_BODY()

    UResource* MakeResource(FName name, int relativeInventorySize, const TCHAR* image, bool constructionResource);

public:
    UResourceBook();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* HardOre;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* LargeParts;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* SmallParts;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* ConductiveOre;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* ConductiveParts;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Oil;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Rubber;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Sand;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Glass;

    // TODO Food
    // TODO Algae
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* MechanicalComponents;    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* ElectricalComponents;    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Cable;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* LifeSupportEquipment;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* People;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Workforce;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Electricity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<UResource*> All;

};
