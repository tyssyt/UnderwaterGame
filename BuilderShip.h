// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ConstructionSite.h"

#include "CoreMinimal.h"
#include "Buildings/XActor.h"
#include "GameFramework/Actor.h"
#include "BuilderShip.generated.h"

enum class ShipState {
    IDLE,
    FLYING
};

UCLASS()
class XD_API ABuilderShip : public AXActor {
    GENERATED_BODY()

public:
    ABuilderShip();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    static float Speed;
    static float SlowSpeed;
    static float RotationSpeed;

    void StartConstructing(ConstructionSite* constructionSite);


protected:

    ShipState State = ShipState::IDLE;
    AActor* NextStop;
    
    ConstructionSite* TargetSite;
    Material Inventory;

    APickupPad* PickupFrom;
    Material PickupMaterial;

    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void Idle();
    void Fly();

    void DoNextStop();

};
