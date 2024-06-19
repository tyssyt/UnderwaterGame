// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TickTimer.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnTimerFinishedSignature);

UCLASS()
class XD_API UTickTimerHandle : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    int Ticks;
    UPROPERTY()
    FOnTimerFinishedSignature OnTimerFinished;
    void Cancel();    
};

UCLASS()
class XD_API UTickTimer : public UObject, public FTickableGameObject {
    GENERATED_BODY()

protected:
    uint32 LastFrameNumberWeTicked = INDEX_NONE;

    UPROPERTY()
    TArray<UTickTimerHandle*> Handles;

public:
    UTickTimerHandle* AddTimer(int ticks);
    void CancelTimer(UTickTimerHandle* handle) { Handles.RemoveSingleSwap(handle); }

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( UTickTimer, STATGROUP_Tickables );
    }
    virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
};
