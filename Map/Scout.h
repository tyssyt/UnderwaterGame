// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapChunk.h"
#include "UObject/Object.h"
#include "Scout.generated.h"

struct FTarget {
    FVector2D Position;
    bool Scan;

    FTarget(const FVector2D position, const bool scan) : Position(position), Scan(scan) {}
};

UCLASS()
class XD_API UScout : public UObject, public FTickableGameObject {
    GENERATED_BODY()

public:    
    static double Speed;

protected:
    UPROPERTY()
    UMapChunk* Chunk;
    TArray<FTarget> Targets;

    uint32 LastFrameNumberWeTicked = INDEX_NONE;

public:
    UPROPERTY()
    FVector2D Position;

    UScout* Init(UMapChunk* chunk, FVector2D position);
    void AddTarget(FVector2D position, bool scan);
    bool RemoveTarget(int column, int row);
    const TArray<FTarget>& GetTargets() { return Targets; } // don't change it please, just iter
    void SanitizeTargets();

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( UConstructionManager, STATGROUP_Tickables );
    }
};
