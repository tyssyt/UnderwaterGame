// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MapCoordinate.h"
#include "Reward/Reward.h"
#include "UObject/Object.h"
#include "Event.generated.h"

class UEncyclopediaEntry;
class UEventStage;
class UEventOption;
class UEventInstance;

UCLASS()
class XD_API UEvent : public UObject {
    GENERATED_BODY()

protected:

    UPROPERTY()
    TArray<UEventStage*> Stages;

    // conditions

    UPROPERTY()
    TArray<UEventInstance*> Instances;

public:
    UPROPERTY()
    FText Name;
    UPROPERTY()
    UTexture2D* Image;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UEncyclopediaEntry* EncyclopediaEntry;

    UEvent* Init(const FText& name, const TCHAR* image);
    void AddStage(const FText& text);
    UEventStage* GetStage(int idx) { return Stages[idx]; }
    
    UEventInstance* CreateInstance(const FMapCoordinate& Coordinate);

    friend class UEncyclopediaPageEvent;
};

// TODO think about if we want to turn some of these into structs, should have less overhead and stuff

UCLASS()
class XD_API UEventStage : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    FText Text;
    UPROPERTY()
    TArray<UEventOption*> Options;

    void AddOption(const FText& text, const TArray<UReward*>& rewards, UEventStage* continuation);
};

UCLASS()
class XD_API UEventOption : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    FText Text;
    UPROPERTY()
    TArray<UReward*> Rewards;
    UPROPERTY()
    UEventStage* Continuation;    
};

UCLASS()
class XD_API UEventInstance : public UObject {
    GENERATED_BODY()    

public:
    UPROPERTY(BlueprintReadOnly)
    UEvent* Event;
    UPROPERTY()
    FMapCoordinate Coordinate;
    UPROPERTY()
    TArray<UEventOption*> ChosenOptions;

    UEventInstance* Init(UEvent* event, const FMapCoordinate& coordinate);
    UFUNCTION()
    void OpenUI();
};