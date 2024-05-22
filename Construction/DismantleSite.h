// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BuilderShip.h"
#include "XD/Buildings/Building.h"
#include "DismantleSite.generated.h"

class UDismantleTask;

UCLASS()
class UGettingDismantled : public UCondition {
    GENERATED_BODY()
public:
    UGettingDismantled();
};

UCLASS()
class XD_API UDismantleSite : public UObject {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UGettingDismantled* Condition;
    UPROPERTY()
    UDismantleTask* Task;

public:
    UPROPERTY()
    ABuilding* Building;

    UDismantleSite* Init(ABuilding* building); 
    void QueueTasks();
    void FinishDismantle() const;
};

UCLASS()
class UDismantleTask : public UBuilderTask {
    GENERATED_BODY()

    UPROPERTY()
    UDismantleSite* DismantleSite;

public:
    enum class EState { FlyToSite, Done } NextCommand;

    UDismantleTask* Init(UDismantleSite* dismantleSite);    
    virtual ABuilderShip::FCommand GetNextCommand() override;    
};
