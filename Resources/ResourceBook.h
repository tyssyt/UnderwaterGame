// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Resource.h"
#include "ResourceBook.generated.h"

UCLASS()
class UResourceBook : public UObject {
    GENERATED_BODY()

public:
    UResourceBook();

    UPROPERTY(VisibleAnywhere)
    UResource* HardOre;
    UPROPERTY(VisibleAnywhere)
    UResource* LargeParts;
    UPROPERTY(VisibleAnywhere)
    UResource* SmallParts;

    UPROPERTY(VisibleAnywhere)
    UResource* ConductiveOre;
    UPROPERTY(VisibleAnywhere)
    UResource* Cable;

    UPROPERTY(VisibleAnywhere)
    UResource* Oil;
    UPROPERTY(VisibleAnywhere)
    UResource* Rubber;
    
    UPROPERTY(VisibleAnywhere)
    UResource* People;
    UPROPERTY(VisibleAnywhere)
    UResource* Workforce;

};
