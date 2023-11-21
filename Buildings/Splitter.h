// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "Conveyor.h"
#include "XD/Inventory/InventoryComponent.h"
#include "Splitter.generated.h"


UCLASS(Abstract)
class XD_API AJunction : public ABuilding {
    GENERATED_BODY()

protected:
    void InternalDisconnect(bool isSplitter, UInventoryComponent* InventoryComponent, AConveyor* conveyor);

public:
    int Connections = 0;
};

UCLASS()
class XD_API ASplitter : public AJunction {
    GENERATED_BODY()

public:
    ASplitter();

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UConveyorNode* Mesh;

    virtual void Tick(float DeltaTime) override;
    void Disconnect(AConveyor* conveyor);
};


UCLASS()
class XD_API AMerger : public AJunction {
    GENERATED_BODY()

public:
    AMerger();

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UConveyorNode* Mesh;

    int Connections = 0;

    virtual void Tick(float DeltaTime) override;
    void Disconnect(AConveyor* conveyor);
};
