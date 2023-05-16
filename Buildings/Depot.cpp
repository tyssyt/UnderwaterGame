// Fill out your copyright notice in the Description page of Project Settings.

#include "Depot.h"
#include "Application/SlateApplicationBase.h"


ADepot::ADepot() {

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/SM_Depot")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
    //Mesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.25f));
    //Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    //Mesh->SetupAttachment(DummyRoot);
    //Mesh->OnClicked.AddDynamic(this, &AP1Block::BlockClicked);
    //Mesh->OnInputTouchBegin.AddDynamic(this, &AP1Block::OnFingerPressedBlock);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->IsBuffer = true;
    Inventory->GetInputs().Emplace(50000);
}

void ADepot::BeginPlay() {
    Super::BeginPlay();
}


void UDepotUI::Tick() {
    if (Depot) {
        FInventorySlot* inv = &Depot->Inventory->GetInputs()[0];
        InventorySlot->Set(inv);
    }
}
