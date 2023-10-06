// Fill out your copyright notice in the Description page of Project Settings.

#include "OilFish.h"

#include "XD/GameInstanceX.h"
#include "XD/PlayerControllerX.h"
#include "XD/Utils.h"
#include "XD/Buildings/OilFishHarvester.h"

AOilFish::AOilFish() {
    PrimaryActorTick.bCanEverTick = false;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    Mesh->SetRelativeScale3D(FVector(1.f, 1.f, .25f));
    SetRootComponent(Mesh);
}

void AOilFish::BeginPlay() {
    Super::BeginPlay();
}


void UOilFishUI::Tick() {
    if (Deposit) {
        // for now, the image is set in the wbp
        // TODO show the resources needed to upgrade and maybe even the building it upgrades to
        // ResourceImage->SetBrushFromTexture(Deposit->GetGameInstance()->TheResourceBook->Oil->Image);
    }
}

void UOilFishUI::OnClickConstruct() {
    if (Deposit) {
        const auto mine = GetWorld()->SpawnActor<AOilFishHarvester>(Deposit->GetActorLocation() - FVector(.0f, .0f, 50.f), Deposit->GetActorRotation()); // TODO figure out why that -50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        The::ConstructionManager(Deposit)->AddConstruction(new ConstructionSite(mine, The::Encyclopedia(Deposit)->OilFishHarvester, FConstructionFlags{true}));
        The::PlayerController(Deposit)->Deselect(); // TODO once we have an UI for construction sites, we can select that instead
        Deposit->Destroy();
        Deposit = nullptr;
        // TODO unselect UI (maybe select the MineUI or sth idk)
    }
}
