// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "Conveyor.h"
#include "XD/Inventory/InventoryComponent.h"
#include "Splitter.generated.h"

UCLASS()
class XD_API ASplitter : public ABuilding {
    GENERATED_BODY()

public:
    ASplitter();

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UConveyorNode* Mesh;

    int Connections = 0;

    virtual void Tick(float DeltaTime) override;
};


UCLASS()
class XD_API AMerger : public ABuilding {
    GENERATED_BODY()

public:
    AMerger();

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UConveyorNode* Mesh;

    int Connections = 0;

    virtual void Tick(float DeltaTime) override;
};
