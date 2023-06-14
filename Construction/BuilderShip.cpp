// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderShip.h"
#include "XD/GameInstanceX.h"

#include "Math/UnrealMathUtility.h"

float ABuilderShip::Speed = 10.f; // why c++ be like this?
float ABuilderShip::SlowSpeed = 3.333f;
float ABuilderShip::RotationSpeed = 1.f;

ABuilderShip::ABuilderShip() {
    PrimaryActorTick.bCanEverTick = true;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/BuilderShip"));
    Mesh->SetStaticMesh(PlaneMesh.Get());
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


static bool logasd = false; // TODO remove ince turn bug is fixed
void ABuilderShip::Fly() {
    const FVector location = GetActorLocation();
    FRotator rotation = GetActorRotation();

    FVector target = NextStop->GetActorLocation();
    target.Z = 150.f;
    const FVector toTarget = target - location;
    const float targetRotation = toTarget.Rotation().Yaw; // TODO there are some toRotator function in vector, try them instead?

    float diff = targetRotation - rotation.Yaw;

    // TODO still sometimes rotates the wrong way round, this actually doen't fix it
    if (diff > 180.f) {
        diff -= 360.f;
    } else if (diff < -180.f) {
        diff += 360.f;
    }

    float speed;
    if (FMath::Abs(diff) < RotationSpeed) {
        speed = Speed;
        rotation.Yaw = targetRotation;
    } else {
        if (logasd) {
            UE_LOG(LogTemp, Warning, TEXT("Rotation: %f"), diff);
            logasd = false;
        }
        speed = SlowSpeed;
        if (targetRotation > 0.f) {
            rotation.Yaw += RotationSpeed;
        } else {
            rotation.Yaw -= RotationSpeed;
        }
    }
    SetActorRotation(rotation);

    const float distance = toTarget.Size();
    if (distance < speed) {
        // We arrived
        SetActorLocation(target);

        if (NextStop == PickupFrom) {
            // collect resource
            for (auto& input : PickupFrom->Inventory->GetInputs()) {
                if (input.Resource == PickupMaterial.resource) {
                    Inventory.amount = input.PullFrom(PickupMaterial.amount);
                    Inventory.resource = PickupMaterial.resource;
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
        
        return;
    }

    const FVector move = (speed / distance) * toTarget;
    SetActorLocation(location + move);
}


void ABuilderShip::StartConstructing(ConstructionSite* constructionSite) {
    SetActorTickEnabled(true);
    TargetSite = constructionSite;
    State = ShipState::FLYING;
    logasd = true;

    DoNextStop();
}

void ABuilderShip::DoNextStop() {
    const auto nextDelivery = TargetSite->GetNextDelivery(&GetGameInstance()->TheConstructionManager->constructionResources);

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
