// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Buildings/XActor.h"

#include "CoreMinimal.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "BuilderShip.generated.h"

class APickupPad;
class UBuilderTask;

UCLASS()
class XD_API ABuilderShip : public AXActor {
    GENERATED_BODY()

public:
    static double Speed;
    static double SlowSpeed;
    static double RotationSpeed;

    struct FCommand {
        FCommand() : Type(EType::Nothing) {}
        FCommand(FVector flyToTarget) : Type(EType::FlyTo), FlyToTarget(flyToTarget) {}
        FCommand(int waitTicks) : Type(EType::Wait), WaitTicks(waitTicks) {}

        enum class EType { Nothing, FlyTo, Wait } Type;
        union {
            FVector FlyToTarget;
            int WaitTicks;
        };
    };

protected:
    UPROPERTY()
    UBuilderTask* Task;
    FCommand Command;

    virtual void Tick(float DeltaTime) override;    
    void FlyTo();
    void Wait();
    void NextCommand();

public:
    ABuilderShip();
    void DoTask(UBuilderTask* task);
};

UCLASS(Abstract)
class XD_API UBuilderTask : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    FVector Location;
    Material RequiredMaterial;
    UPROPERTY()
    APickupPad* PickupFrom; // set by the construction manager, the material is already reserved for us in there

    virtual ABuilderShip::FCommand GetNextCommand() { return ABuilderShip::FCommand(); }
};
