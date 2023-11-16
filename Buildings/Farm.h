// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "Farm.generated.h"

// TODO not sure if this should really live in this header file
struct CropData {
    CropData(
        const TArray<const TCHAR*>& Meshes,
        const int Rows,
        const int Columns,
        const FVector& PosRandom,
        const FVector& ScaleBase,
        const FVector& ScaleRandom
    ) : Meshes(Meshes), Rows(Rows), Columns(Columns), PosRandom(PosRandom), ScaleBase(ScaleBase), ScaleRandom(ScaleRandom) {}

    TArray<const TCHAR*> Meshes;
    int Rows;
    int Columns;

    FVector PosRandom; // position will be uniform randomized between -PosRandom and PosRandom,
    FVector ScaleBase; // Scale will be randomized between ScaleBase and ScaleBase + ScaleRandom
    FVector ScaleRandom;
};

UCLASS()
class XD_API AFarm : public ABuilding {
    GENERATED_BODY()

protected:
    enum class AnimationState { Forward, WaitMin, Backward, WaitMax };
    AnimationState AnimationState = AnimationState::Forward;
    int TicksWaited = 0;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* CraneMesh;

public:
    AFarm();
    virtual void Tick(float DeltaTime) override;

    void SetCrop(CropData* cropData);
};
