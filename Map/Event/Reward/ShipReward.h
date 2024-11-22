// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Reward.h"
#include "ShipReward.generated.h"

UCLASS()
class XD_API UShipReward : public UReward {
    GENERATED_BODY()

public:
    virtual void Give(UEventInstance* event) override;
};
