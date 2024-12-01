// Fill out your copyright notice in the Description page of Project Settings.

#include "MapCoordinate.h"

#include "MapChunk.h"

bool FMapCoordinate::IsRevealed() const {
    return Chunk->IsRevealed(Column, Row);
}
