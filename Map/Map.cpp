// Fill out your copyright notice in the Description page of Project Settings.

#include "Map.h"

#include "The.h"
#include "XD/BlueprintHolder.h"

UMap* UMap::Init() {
    CurrentChunk = NewObject<UMapChunk>(this);
    return this;
}

void UMap::AddScout(const FMapCoordinate coordinate) {
    const auto scout = NewObject<UScout>(this)->Init(coordinate.Chunk, { coordinate.Column + .5, coordinate.Row + .5 });
    The::BPHolder(this)->MapUI->AddScout(scout);
    Scouts.Add(scout);
}

FString UMap::CoordinateToString(int column, int row) {
    constexpr int center = UMapChunk::CHUNK_SIZE/2;
    column -= center;
    row -= center;

    FString string;
    string += FText::AsNumber(FMath::Abs(row), &FNumberFormattingOptions::DefaultNoGrouping()).ToString();
    if (row <= 0)
        string += "N ";
    else 
        string += "S ";

    string += FText::AsNumber(FMath::Abs(column), &FNumberFormattingOptions::DefaultNoGrouping()).ToString();
    if (column <= 0)
        string += "W";
    else
        string += "E";

    return MoveTemp(string);
}
