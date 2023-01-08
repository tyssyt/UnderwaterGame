// Fill out your copyright notice in the Description page of Project Settings.

#include "ConductiveOreMine.h"

#include "XD/GameInstanceX.h"
#include "XD/Resources/ResourceBook.h"

AConductiveOreMine::AConductiveOreMine() {
    PrimaryActorTick.bCanEverTick = true;

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

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AConductiveOreMine::BeginPlay() {
    Super::BeginPlay();

    Inventory->GetOutputs().Emplace(ProductionPerTick * 10, GetGameInstance()->TheResourceBook->ConductiveOre);
    ProductionPerTick = 20;
    Inventory->GetOutputs()[0].Max = 200;
}

void AConductiveOreMine::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    Inventory->GetOutputs()[0].PushInto(ProductionPerTick);
}

void UConductiveOreMineUI::Tick() {
    if (Mine) {
        ProductionPerTick->SetText(FText::AsNumber(Mine->ProductionPerTick * 120)); // TODO use actual ticks/s
        ResourceImage->SetBrushFromTexture(Mine->Inventory->GetOutputs()[0].Resource->Image); // TODO save the resource somwhere else???
    }
}
