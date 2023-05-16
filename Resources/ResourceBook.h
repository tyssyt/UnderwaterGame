// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Resource.h"
#include "ResourceBook.generated.h"

UCLASS()
class UResourceBook : public UObject {
    GENERATED_BODY()

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
    UResource* Cable;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Oil;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Rubber;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* People;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UResource* Workforce;

};
