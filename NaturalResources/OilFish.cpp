// Fill out your copyright notice in the Description page of Project Settings.

#include "OilFish.h"

#include "XD/GameInstanceX.h"
#include "XD/Buildings/OilFishHarvester.h"
#include "XD/Resources/ResourceBook.h"

AOilFish::AOilFish() {
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
}

void AOilFish::BeginPlay() {
    Super::BeginPlay();
}


void UOilFishUI::Tick() {
    if (Deposit) {
        // TODO we propably want a getter for conductive ore in the Deposit or something like that...
        ResourceImage->SetBrushFromTexture(Deposit->GetGameInstance()->TheResourceBook->Oil->Image);
    }
}

void UOilFishUI::OnClickConstruct() {
    if (Deposit) {
        UWorld* world = Deposit->GetWorld();
        auto mine = world->SpawnActor<AOilFishHarvester>(Deposit->GetActorLocation() - FVector(.0f, .0f, 50.f), Deposit->GetActorRotation()); // TODO figure out why that -50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        world->GetGameInstance<UGameInstanceX>()->TheConstructionManager->AddConstruction(mine);
        Deposit->Destroy();
        Deposit = nullptr;
        // TODO unselect UI (maybe select the MineUI or sth idk)
    }
}
