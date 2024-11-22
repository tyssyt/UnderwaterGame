// Fill out your copyright notice in the Description page of Project Settings.


#include "ScanReward.h"

#include "XD/Map/MapChunk.h"
#include "XD/Map/Event/Event.h"

void UScanReward::Give(UEventInstance* event) {
    switch (Direction) {
    case EScanDirection::West:
        event->Coordinate.Chunk->Reveal(event->Coordinate.Column -1, event->Coordinate.Row);
        break;
    case EScanDirection::North:
        event->Coordinate.Chunk->Reveal(event->Coordinate.Column, event->Coordinate.Row -1);
        break;
    case EScanDirection::East:
        event->Coordinate.Chunk->Reveal(event->Coordinate.Column +1, event->Coordinate.Row);
        break;
    case EScanDirection::South:
        event->Coordinate.Chunk->Reveal(event->Coordinate.Column, event->Coordinate.Row +1);
        break;
    }
}
