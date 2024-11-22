// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Reward.generated.h"

class UEventInstance;

UCLASS(Abstract)
class XD_API UReward : public UObject {
    GENERATED_BODY()

public:
    virtual void Give(UEventInstance* event) {}
};
