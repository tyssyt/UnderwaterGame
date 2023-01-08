// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BillboardComponent.h"
#include "ElectricComponent.generated.h"

enum class PowerState { Initial, Disconnected, Unpowered, Powered };

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UElectricComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UElectricComponent();

    UPROPERTY(EditAnywhere)
    int Consumption;

    // TODO enum for state, under_construction, disconnected (no network nearby with enough production, will reconnect if possible, I think we need 2 states, nothing nearby and something nearby with not enough power...), manually_disconnected, powered

    UPROPERTY(EditAnywhere)
    class ASubstation* Substation;

    UPROPERTY(EditAnywhere)
    UBillboardComponent* DisabledSymbol;

    PowerState GetState();
    void SetState(PowerState newState);

protected:
    PowerState State;

};
