// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/InventorySlot.h"

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UInventoryComponent : public UActorComponent {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere)
    TArray<FInventorySlot> Inputs;

    UPROPERTY(EditAnywhere)
    TArray<FInventorySlot> Outputs;

public:
    UInventoryComponent();

    bool IsBuffer = false;
    TArray<FInventorySlot>& GetInputs();
    TArray<FInventorySlot>& GetOutputs();

public:
};
