// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ConveyorGate.generated.h"

class AConveyor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UConveyorGate : public UStaticMeshComponent {
    GENERATED_BODY()

protected:
    bool bIsInput = false;

public:    
    UPROPERTY(VisibleAnywhere)
    AConveyor* Conveyor;

    UConveyorGate();

    bool IsInput() const { return bIsInput; }
    UFUNCTION()
    void SetInput(bool isInput) { bIsInput = isInput; }

    UFUNCTION()
    void SetLocation(FVector location) { SetRelativeLocation(location); }
    UFUNCTION()
    void SetRotation(double rotation) { SetRelativeRotation(FRotator(0., rotation, 0.)); }
};
