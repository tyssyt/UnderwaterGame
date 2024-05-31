// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "GameFramework/Actor.h"
#include "IndoorBuilding.generated.h"

class AHabitat;
typedef TPair<int, int> Coordinate;

enum class EGridOrientation { North, East, West, South };

UCLASS()
class XD_API AIndoorBuilding : public ABuilding {
    GENERATED_BODY()

public:
    UPROPERTY()
    AHabitat* Habitat;
    int GridX;
    int GridY;
    EGridOrientation Orientation;

    void Rotate();
    virtual const TArray<Coordinate>* GetGridOffsets();
    virtual void SetCoordinates(int x, int y, AHabitat* habitat);

    AIndoorBuilding();
    virtual AIndoorBuilding* Init(UConstructionPlan* constructionPlan) override;

    virtual void OnDismantleFinish() override;
};
