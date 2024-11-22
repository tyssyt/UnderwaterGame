// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Event/Event.h"
#include "UObject/Object.h"
#include "MapChunk.generated.h"

UCLASS()
class XD_API UMapChunk : public UObject {
    GENERATED_BODY()

public:
    static constexpr int CHUNK_SIZE = 11; // 64
    static constexpr int TICKS_TO_SCAN = 600;

protected:
    bool Sectors[CHUNK_SIZE*CHUNK_SIZE];
    static int ToIdx(int column, int row);

    TMap<int, int> ScanProgress;

    UPROPERTY()
    TArray<UEventInstance*> Events;

    // list of biomes and their coordinates
    // in theory, the neighbor chunks but we will not do that yet

    // maybe texture or generation paremeters
public:
    void AddEvent(UEventInstance* event) { Events.Add(event); }
    bool IsRevealed(int column, int row) const;

    bool Scan(int column, int row);
    void Reveal(int column, int row);
};