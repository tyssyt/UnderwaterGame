// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapCoordinate.generated.h"

class UMapChunk;

USTRUCT()
struct XD_API FMapCoordinate {
    GENERATED_BODY()

    UPROPERTY()
    UMapChunk* Chunk;
    UPROPERTY()
    int Row;
    UPROPERTY()
    int Column;

    bool IsRevealed() const;
};