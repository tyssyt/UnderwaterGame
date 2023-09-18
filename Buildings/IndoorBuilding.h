// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "Building.h"
#include "XActor.h"
#include "GameFramework/Actor.h"
#include "IndoorBuilding.generated.h"

typedef std::pair<int, int> Coordinate;

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
    virtual const std::vector<Coordinate>* getGridOffsets();
    virtual void setCoordinates(int x, int y, AHabitat* habitat);

protected:
    virtual void BeginPlay() override;

};
