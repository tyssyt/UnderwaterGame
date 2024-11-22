// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapChunk.h"
#include "Scout.h"
#include "UObject/Object.h"
#include "Map.generated.h"

UCLASS()
class XD_API UMap : public UObject {
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<UScout*> Scouts;
    
public:
    // transports
    UPROPERTY()
    UMapChunk* CurrentChunk;

    UMap* Init();
    void AddScout(const FMapCoordinate coordinate);

    static FString CoordinateToString(int column, int row);
};
