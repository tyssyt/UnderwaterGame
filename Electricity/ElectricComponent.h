// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BillboardComponent.h"
#include "ElectricComponent.generated.h"

class ASubstation;

enum class PowerState { Initial, Disconnected, Deactivated, Unpowered, Powered };

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UElectricComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UElectricComponent();

    UPROPERTY(EditAnywhere)
    int Consumption;

    UPROPERTY(EditAnywhere)
    ASubstation* Substation;

    UPROPERTY(EditAnywhere)
    UBillboardComponent* DisabledSymbol;

    PowerState GetState() const;
    void SetState(const PowerState newState);

protected:
    PowerState State;
};
