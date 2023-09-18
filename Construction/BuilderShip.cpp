// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderShip.h"
#include "XD/GameInstanceX.h"

#include "Math/UnrealMathUtility.h"

double ABuilderShip::Speed = 10.f; // why c++ be like this?
double ABuilderShip::SlowSpeed = 3.333f;
double ABuilderShip::RotationSpeed = 1.f;

ABuilderShip::ABuilderShip() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/BuilderShip"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    SetRootComponent(Mesh);
}

void ABuilderShip::BeginPlay() {
    Super::BeginPlay();
}

void ABuilderShip::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    switch (State) {
    case ShipState::IDLE:
        Idle();
        break;
    case ShipState::FLYING:
        Fly();
        break;
    }
}

void ABuilderShip::Idle() {
    SetActorTickEnabled(false);
    GetGameInstance()->TheConstructionManager->AddIdleBuilder(this);
}


void ABuilderShip::Fly() {
    const FVector location = GetActorLocation();

    FVector target = NextStop->GetActorLocation();
    target.Z = 150.f;
    const FVector toTarget = target - location;
    const double targetRotation = toTarget.Rotation().Yaw;

    double rotDiff = targetRotation - GetActorRotation().Yaw;
    while (rotDiff < -180.)
        rotDiff += 360.;
    while (rotDiff > 180.)
        rotDiff -= 360.;

    double speed;
    if (FMath::Abs(rotDiff) < RotationSpeed) {
        speed = Speed;
        SetActorRotation(FRotator(0., targetRotation, 0.));
    } else {
        speed = SlowSpeed;
        if (rotDiff > 0.f) {
            AddActorWorldRotation(FRotator(0., RotationSpeed, 0.));
        } else {
            AddActorWorldRotation(FRotator(0., -RotationSpeed, 0.));
        }
    }

    const double distance = toTarget.Size();
    if (distance > speed) {
        AddActorWorldOffset(speed / distance * toTarget);
        return;
    }
    
    // We arrived
    SetActorLocation(target);

    if (NextStop == PickupFrom) {
        // collect resource
        for (auto& input : PickupFrom->Inventory->GetInputs()) {
            if (input.Resource == PickupMaterial.resource) {
                Inventory.resource = PickupMaterial.resource;
                Inventory.amount = input.PullFrom(PickupMaterial.amount);
                GetGameInstance()->TheConstructionManager->UnreserveResource(Inventory.resource, Inventory.amount);
            }
        }
        
        NextStop = TargetSite->Building;
    } else if (PickupMaterial.resource) {
        // deliver resource
        TargetSite->DeliverMaterial(Inventory);
        Inventory.amount = 0;
        Inventory.resource = nullptr;
        DoNextStop();
    } else {
        // start construction
        TargetSite->BeginConstruction();
        State = ShipState::IDLE;
    }

}


void ABuilderShip::StartConstructing(ConstructionSite* constructionSite) {
    SetActorTickEnabled(true);
    TargetSite = constructionSite;
    State = ShipState::FLYING;

    DoNextStop();
}

void ABuilderShip::DoNextStop() {
    const auto nextDelivery = TargetSite->GetNextDelivery(GetGameInstance()->TheConstructionManager->constructionResources);

    if (nextDelivery.first) {
        NextStop = nextDelivery.first;
        PickupFrom = nextDelivery.first;
        PickupMaterial = nextDelivery.second;        
    } else {
        // all Material is delivered, go to Build Site and finish it
        NextStop = TargetSite->Building;
        PickupFrom = nullptr;
        PickupMaterial.amount = 0;
        PickupMaterial.resource = nullptr;
    }
}
