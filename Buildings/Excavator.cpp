// Fill out your copyright notice in the Description page of Project Settings.

#include "Excavator.h"

#include "XD/GameInstanceX.h"
#include "XD/Resources/ResourceBook.h"

AExcavator::AExcavator() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Excavator/Excavator_Base"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    SetRootComponent(Mesh);
    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> WheelMeshFinder(TEXT("/Game/Assets/Meshes/Excavator/Excavator_Wheel"));
    WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh1"));
    WheelMesh->SetStaticMesh(WheelMeshFinder.Object);
    WheelMesh->SetupAttachment(Mesh);

    // TODO fix the model such that the wheel no longer clips the ground
    //WheelMesh->AddLocalOffset(FVector(102.62, 0., 13.099));
    WheelMesh->SetRelativeScale3D(FVector(0.9, 1, 0.9));
    WheelMesh->AddLocalOffset(FVector(102.62, 0., 15.));

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AExcavator::BeginPlay() {
    Super::BeginPlay();

    Inventory->GetOutputs().Emplace(ProductionPerTick * 10, GetGameInstance()->TheResourceBook->Sand);
    ProductionPerTick = 20;
    Inventory->GetOutputs()[0].Max = 200;
}

void AExcavator::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    Inventory->GetOutputs()[0].PushInto(ProductionPerTick);

    WheelMesh->AddLocalRotation(FRotator(1., 0., 0.)); // TODO something frame rate?
}

void UExcavatorUI::Tick() {
    if (Excavator) {
        ProductionPerTick->SetText(FText::AsNumber(Excavator->ProductionPerTick * 120)); // TODO use actual ticks/s
        ResourceImage->SetBrushFromTexture(Excavator->Inventory->GetOutputs()[0].Resource->Image); // TODO save the resource somwhere else???
    }
}
