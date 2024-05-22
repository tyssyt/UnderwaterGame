// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupPad.h"

#include "The.h"
#include "XD/Construction/ConstructionManager.h"
#include "XD/Inventory/ConveyorGate.h"

APickupPad::APickupPad() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Pickup_Pad"));
    const auto mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    mesh->SetStaticMesh(MeshFinder.Object);
    mesh->SetRenderCustomDepth(true);
    SetRootComponent(mesh);

    const auto conveyorGate1 = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate1"));
    conveyorGate1->SetInput(true);
    conveyorGate1->SetRelativeLocation(FVector(39., 0., 8.));
    conveyorGate1->SetRenderCustomDepth(true);
    conveyorGate1->SetupAttachment(mesh);

    const auto conveyorGate2 = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate2"));
    conveyorGate2->SetInput(true);
    conveyorGate2->SetRelativeLocation(FVector(0., 39., 8.));
    conveyorGate2->SetRelativeRotation(FRotator(0., 90., 0.));
    conveyorGate2->SetRenderCustomDepth(true);
    conveyorGate2->SetupAttachment(mesh);

    const auto conveyorGate3 = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate3"));
    conveyorGate3->SetInput(true);
    conveyorGate3->SetRelativeLocation(FVector(-39., 0., 8.));
    conveyorGate3->SetRelativeRotation(FRotator(0., 180., 0.));
    conveyorGate3->SetRenderCustomDepth(true);
    conveyorGate3->SetupAttachment(mesh);

    const auto conveyorGate4 = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate4"));
    conveyorGate4->SetInput(true);
    conveyorGate4->SetRelativeLocation(FVector(0., -39., 8.));
    conveyorGate4->SetRelativeRotation(FRotator(0., -90., 0.));
    conveyorGate4->SetRenderCustomDepth(true);
    conveyorGate4->SetupAttachment(mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Inventory->GetInputs().Emplace(2500);
    Inventory->GetInputs().Emplace(2500);
    Inventory->GetInputs().Emplace(2500);
    Inventory->GetInputs().Emplace(2500);
    AddInstanceComponent(Inventory);
}

int APickupPad::GetUnreserved(const UResource* resource) const {
    if (const auto inv = Inventory->GetInput(resource)) {
        if (const auto reserved = Material::Find(ReservedMaterials, resource))
            return FMath::Max(0, inv->Current - reserved->amount);
        return inv->Current;
    }
    return 0;
}

void APickupPad::OnConstructionComplete(UBuilderModeExtensions* extensions) {
    Super::OnConstructionComplete(extensions);
    The::ConstructionManager(this)->AddPickupPad(this);
}

void APickupPad::OnDismantle() {
    Super::OnDismantle();
    The::ConstructionManager(this)->RemovePickupPad(this);
}
