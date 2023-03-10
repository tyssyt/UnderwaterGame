// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderShip.h"
#include "GameInstanceX.h"
#include "Math/UnrealMathUtility.h"

float ABuilderShip::Speed = 1.f; // why c++ be like this?
float ABuilderShip::SlowSpeed = .333f;
float ABuilderShip::RotationSpeed = 1.f;

ABuilderShip::ABuilderShip() {
    PrimaryActorTick.bCanEverTick = true;

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/BuilderShip")), BaseMaterial(TEXT("/Game/BasicShapeMaterial")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
    Mesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
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
    FVector location = GetActorLocation();
    FRotator rotation = GetActorRotation();

    FVector target = TargetSite->Building->GetActorLocation();
    target.Z = 150.f;
    FVector toTarget = target - location;
    float targetRotation = FRotationMatrix::MakeFromX(toTarget).Rotator().Yaw; // TODO there are some toRotator function in vector, try them instead?

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

    float distance = toTarget.Size();
    if (distance < speed) {
        SetActorLocation(target);
        TargetSite->BeginConstruction();
        State = ShipState::IDLE;
        return;
    }

    FVector move = (speed / distance) * toTarget;
    SetActorLocation(location + move);

}

void ABuilderShip::StartConstructing(ConstructionSite* constructionSite) {
    SetActorTickEnabled(true);
    TargetSite = constructionSite;
    State = ShipState::FLYING;
    logasd = true;
}
