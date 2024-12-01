// Fill out your copyright notice in the Description page of Project Settings.

#include "MapChunk.h"

#include "The.h"
#include "XD/BlueprintHolder.h"

int UMapChunk::ToIdx(int column, int row) {
    return column*CHUNK_SIZE + row;
}

bool UMapChunk::IsRevealed(int column, int row) const {
    return Sectors[ToIdx(column, row)];
}

bool UMapChunk::Scan(int column, int row) {
    const int idx = ToIdx(column, row);
    if (auto counter = ScanProgress.Find(idx)) {
        *counter += 1;
        if (*counter >= TICKS_TO_SCAN) {
            ScanProgress.Remove(idx);
            Reveal(column, row);
            return true;
        }
    } else {
        check(!IsRevealed(column, row));
        ScanProgress.Add(idx, 1);
    }
    return false;
}

void UMapChunk::Reveal(int column, int row) {
    if (IsRevealed(column, row))
        return;
    Sectors[ToIdx(column, row)] = true;

    const auto mapUI = The::BPHolder(this)->MapUI;
    mapUI->RevealSector(column, row);

    for (const auto event : Events) {
        if (event->Coordinate.Column != column || event->Coordinate.Row != row)
            continue;

        mapUI->AddEvent(event);
        event->Event->EncyclopediaEntry->Update();
        // pop a message somewhere in the ui
    }

}
