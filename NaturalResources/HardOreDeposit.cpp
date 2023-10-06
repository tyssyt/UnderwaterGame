// Fill out your copyright notice in the Description page of Project Settings.

#include "HardOreDeposit.h"

#include "XD/GameInstanceX.h"
#include "XD/PlayerControllerX.h"
#include "XD/Utils.h"
#include "XD/Buildings/HardOreMine.h"

AHardOreDeposit::AHardOreDeposit() {
    PrimaryActorTick.bCanEverTick = false;
    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    Mesh->SetRelativeScale3D(FVector(1.f, 1.f, .25f));
    SetRootComponent(Mesh);
}

void AHardOreDeposit::BeginPlay() {
    Super::BeginPlay();
}


void UHardOreDepositUI::Tick() {
    if (Deposit) {
        // for now, the image is set in the wbp
        // TODO show the resources needed to upgrade and maybe even the building it upgrades to
        // ResourceImage->SetBrushFromTexture(Deposit->GetGameInstance()->TheResourceBook->HardOre->Image);
    }
}

void UHardOreDepositUI::OnClickConstruct() {
    if (Deposit) {
        const auto mine = GetWorld()->SpawnActor<AHardOreMine>(Deposit->GetActorLocation() - FVector(.0f, .0f, 50.f), Deposit->GetActorRotation()); // TODO figure out why that -50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        The::ConstructionManager(Deposit)->AddConstruction(new ConstructionSite(mine, The::Encyclopedia(Deposit)->HardOreMine, FConstructionFlags{true}));
        The::PlayerController(Deposit)->Deselect(); // TODO once we have an UI for construction sites, we can select that instead
        Deposit->Destroy();
        Deposit = nullptr;
    }
}
