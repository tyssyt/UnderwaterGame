// Fill out your copyright notice in the Description page of Project Settings.

#include "ConductiveOreDeposit.h"

#include "XD/GameInstanceX.h"
#include "XD/Buildings/ConductiveOreMine.h"
#include "XD/Resources/ResourceBook.h"

AConductiveOreDeposit::AConductiveOreDeposit() {
    PrimaryActorTick.bCanEverTick = false;

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Cylinder")), BaseMaterial(TEXT("/Game/BasicShapeMaterial")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    Mesh->SetRelativeScale3D(FVector(1.f, 1.f, .25f));
    Mesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
    SetRootComponent(Mesh);
}

void AConductiveOreDeposit::BeginPlay() {
    Super::BeginPlay();
}


void UConductiveOreDepositUI::Tick() {
    if (Deposit) {
        // TODO we propably want a getter for conductive ore in the Deposit or something like that...
        ResourceImage->SetBrushFromTexture(Deposit->GetGameInstance()->TheResourceBook->ConductiveOre->Image);
    }
}

void UConductiveOreDepositUI::OnClickConstruct() {
    if (Deposit) {
        const UGameInstanceX* gameInstance = Deposit->GetGameInstance();
        const auto mine = Deposit->GetWorld()->SpawnActor<AConductiveOreMine>(Deposit->GetActorLocation() - FVector(.0f, .0f, 50.f), Deposit->GetActorRotation()); // TODO figure out why that -50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        gameInstance->TheConstructionManager->AddConstruction(new ConstructionSite(mine, gameInstance->TheBuildingBook->ConductiveOreMine));
        Deposit->Destroy();
        Deposit = nullptr;
        // TODO unselect UI (maybe select the MineUI or sth idk)
    }
}
