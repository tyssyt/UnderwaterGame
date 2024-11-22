// Fill out your copyright notice in the Description page of Project Settings.

#include "Scout.h"

#include "The.h"
#include "XD/BlueprintHolder.h"

double UScout::Speed = 0.001f; // why c++ be like this?

UScout* UScout::Init(UMapChunk* chunk, FVector2D position) {
    Chunk = chunk;
    Position = position;
    return this;
}

void UScout::AddTarget(FVector2D position, bool scan) {
    if (!Targets.IsEmpty() && !Targets.Last().Scan)
        Targets.RemoveAt(Targets.Num()-1);
    Targets.Emplace(position, scan);
    The::BPHolder(this)->MapUI->UpdateWaypoints(this);
}

bool UScout::RemoveTarget(int column, int row) {    
    const int removed = Targets.RemoveAll([column, row](const FTarget& target) {
        return target.Scan && static_cast<int>(target.Position.X) == column && static_cast<int>(target.Position.Y) == row;
    });
    if (removed > 0) {
        The::BPHolder(this)->MapUI->UpdateWaypoints(this);
        return true;
    }
    return false;
}

void UScout::SanitizeTargets() {
    const int removed = Targets.RemoveAll([&](const FTarget& target) {
        return target.Scan && Chunk->IsRevealed(target.Position.X, target.Position.Y);
    });
    if (removed > 0)
        The::BPHolder(this)->MapUI->UpdateWaypoints(this);
}

bool isInSector(FVector2D pos, FVector2D sector) {
    double column = FMath::Floor(sector.X);
    if (pos.X < column || pos.X > column+1.)
        return false;

    double row = FMath::Floor(sector.Y);
    if (pos.Y < row || pos.Y > row+1.)
        return false;

    return true;
}
double randomDecimals(double in) {
    return FMath::Floor(in) + FMath::FRand();
}
FVector2D randomDecimals(FVector2D in) {
    return {randomDecimals(in.X), randomDecimals(in.Y)};
}
void UScout::Tick(float DeltaTime) {
    // TODO if this never explodes, we can remove it at some point
    if (LastFrameNumberWeTicked == GFrameCounter) {
        checkNoEntry();
        return;
    }

    if (Targets.IsEmpty())
        return;
    FTarget nextTarget = Targets[0];

    while (nextTarget.Scan && Chunk->IsRevealed(nextTarget.Position.X, nextTarget.Position.Y)) {
        Targets.RemoveAt(0);
        The::BPHolder(this)->MapUI->UpdateWaypoints(this);
        if (Targets.IsEmpty())
            return;
        nextTarget = Targets[0];
    }

    if (nextTarget.Scan && isInSector(Position, nextTarget.Position)) {
        if (Chunk->Scan(nextTarget.Position.X, nextTarget.Position.Y)) {
            Targets.RemoveAt(0);
            The::BPHolder(this)->MapUI->UpdateWaypoints(this);
            return;
        }
    }

    // move towards target
    const auto toTarget = nextTarget.Position - Position;
    const double distance = toTarget.Size();
    if (distance > Speed) {
        Position += Speed / distance * toTarget;
        return;
    }

    // we arrived
    Position = nextTarget.Position;
    if (nextTarget.Scan) {
        nextTarget.Position = randomDecimals(nextTarget.Position);
    } else {
        Targets.RemoveAt(0);
        The::BPHolder(this)->MapUI->UpdateWaypoints(this);
    }
}
