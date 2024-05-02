// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"
#include "WireComponent.generated.h"

class ABuilding;

UCLASS()
class XD_API UWireComponent : public USplineMeshComponent {
    GENERATED_BODY()

    UPROPERTY()
    ABuilding* Start;
    UPROPERTY()
    ABuilding* End;

public:
    UWireComponent();
    static UWireComponent* Create(AActor* wireParent, ABuilding* start, ABuilding* end);

    ABuilding* GetStart() const {return Start;}
    ABuilding* GetEnd() const {return End;}

    void SetStart(ABuilding* start, bool bUpdateMesh = true);
    void SetEnd(ABuilding* end, bool bUpdateMesh = true);
};
