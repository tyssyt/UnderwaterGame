// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Reward.h"
#include "ScanReward.generated.h"

UENUM()
enum class EScanDirection { West, North, East, South };

UCLASS()
class XD_API UScanReward : public UReward {
    GENERATED_BODY()

protected:
    UPROPERTY()
    EScanDirection Direction;
    
public:
    virtual void Give(UEventInstance* event) override;
};
