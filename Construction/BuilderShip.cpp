// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderShip.h"

#include "ConstructionManager.h"
#include "The.h"

double ABuilderShip::Speed = 10.f; // why c++ be like this?
double ABuilderShip::SlowSpeed = 3.333f;
double ABuilderShip::RotationSpeed = 1.f;

void ABuilderShip::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    switch (Command.Type) {
    case FCommand::EType::Nothing:
        SetActorTickEnabled(false);
        break;
    case FCommand::EType::FlyTo:
        FlyTo();
        break;
    case FCommand::EType::Wait:
        Wait();
        break;
    }
}

void ABuilderShip::FlyTo() {
    const FVector location = GetActorLocation();

    FVector target = Command.FlyToTarget;
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
    NextCommand();
}

void ABuilderShip::Wait() {    
    if (Command.WaitTicks-- <= 0)
        NextCommand();
}

void ABuilderShip::NextCommand() {
    if (!Task) {
        Command.Type = FCommand::EType::Nothing;
        return;
    }

    Command = Task->GetNextCommand();
    if (Command.Type == FCommand::EType::Nothing) {
        Task = nullptr;

        const auto constructionManager = The::ConstructionManager(this);
        constructionManager->AddIdleBuilder(this);
        if (const auto pickupPad = constructionManager->GetNearestPickupPad(GetActorLocation())) {
            Command = FCommand(pickupPad->GetActorLocation());
        }
    }
}

ABuilderShip::ABuilderShip() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/BuilderShip"));
    const auto mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    mesh->SetStaticMesh(MeshFinder.Object);
    mesh->SetRenderCustomDepth(true);
    SetRootComponent(mesh);
}

void ABuilderShip::DoTask(UBuilderTask* task) {
    check(Task == nullptr);
    Task = task;
    SetActorTickEnabled(true);
    NextCommand();
}
