// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "GameFramework/Actor.h"
#include "IndoorBuilding.generated.h"

typedef TPair<int, int> Coordinate;

enum class EGridOrientation { North, East, West, South };

UCLASS()
class XD_API AIndoorBuilding : public ABuilding {
    GENERATED_BODY()

public:
    AIndoorBuilding();

    class AHabitat* Habitat;
    int GridX;
    int GridY;
    EGridOrientation Orientation;

    virtual void Tick(float DeltaTime) override;

    void Rotate();
    virtual const TArray<Coordinate>* GetGridOffsets();
    virtual void SetCoordinates(int x, int y, AHabitat* habitat);

protected:
    virtual void BeginPlay() override;

};
