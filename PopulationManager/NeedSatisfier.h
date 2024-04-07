// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Need.h"
#include "ResourceConsumption.h"
#include "UObject/Object.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "NeedSatisfier.generated.h"

UCLASS()
class XD_API UNeedSatisfier : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere)
    UNeed* Need;

    TArray<ResourceConsumption> Goods;
    UPROPERTY(VisibleAnywhere)
    TArray<UConstructionPlan*> Services;

    int IsSatisfied(const AHabitat* habitat, int population);
};
