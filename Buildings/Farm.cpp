// Fill out your copyright notice in the Description page of Project Settings.

#include "Farm.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "XD/Inventory/ConveyorGate.h"

AFarm::AFarm() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Farm/Farm"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> CraneMeshFinder(TEXT("/Game/Assets/Meshes/Farm/FarmCrane"));
    CraneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh1"));
    CraneMesh->SetStaticMesh(CraneMeshFinder.Object);
    CraneMesh->SetRenderCustomDepth(true);
    CraneMesh->SetupAttachment(Mesh);

    const auto conveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate"));
    conveyorGate->SetRelativeLocation(FVector(103.33, 0., 8.));
    conveyorGate->SetRenderCustomDepth(true);
    conveyorGate->SetupAttachment(Mesh);
}


void AFarm::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // TODO use an actual animation
    switch (AnimationState) {
    case AnimationState::Forward:
        CraneMesh->AddLocalOffset(FVector(.1, 0., 0.));
        if (CraneMesh->GetRelativeLocation().X > 60.) {
            CraneMesh->SetRelativeLocation(FVector(60., 0., 0.));
            AnimationState = AnimationState::WaitMax;
            TicksWaited = 0;
        }
        break;
    case AnimationState::WaitMin:
        TicksWaited++;
        if (TicksWaited > 60)
            AnimationState = AnimationState::Forward;
        break;
    case AnimationState::Backward:
        CraneMesh->AddLocalOffset(FVector(-.1, 0., 0.));
        if (CraneMesh->GetRelativeLocation().X < -60.) {
            CraneMesh->SetRelativeLocation(FVector(-60., 0., 0.));
            AnimationState = AnimationState::WaitMin;
            TicksWaited = 0;
        }
        break;
    case AnimationState::WaitMax:
        TicksWaited++;
        if (TicksWaited > 60)
            AnimationState = AnimationState::Backward;
        break;
    default: checkNoEntry();
    }
}

double GetStartOffset(double width, double rowDist) {
    return (rowDist - width) / 2;
}

void AFarm::SetCrop(CropData* cropData) {
    static constexpr double width = 70.; // y from -35 to 35
    static constexpr double height = 120.; // x from -60 to 60

    TInlineComponentArray<UHierarchicalInstancedStaticMeshComponent*> oldCrops;
    GetComponents<UHierarchicalInstancedStaticMeshComponent>(oldCrops, true);
    check(oldCrops.IsEmpty()); // TODO remove old crops

    TArray<UHierarchicalInstancedStaticMeshComponent*> crops;
    for (const auto cropMesh : cropData->Meshes) {
        // TODO @ComponentCreation
        const auto crop = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);
        crop->RegisterComponent();
        crop->AttachToComponent(Mesh, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
        crop->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, cropMesh));
        AddInstanceComponent(crop);
        crops.Push(crop);
    }

    const double rowDist = width / cropData->Rows;
    const double columnDist = height / cropData->Columns;

    FVector cur = FVector(GetStartOffset(height, columnDist), GetStartOffset(width, rowDist), 4.);

    for (int x=0; x < cropData->Columns; ++x) {
        for (int y=0; y < cropData->Rows; ++y) {
            FRotator rotation(0., FMath::RandRange(0., 360.), 0.);
            FVector offset = cur + FMath::RandRange(-1., 1.) * cropData->PosRandom;
            FVector scale = cropData->ScaleBase + FMath::FRand()*cropData->ScaleRandom;
            const int variation = FMath::RandRange(0, crops.Num()-1);
            crops[variation]->AddInstance(FTransform(rotation, offset, scale));
            cur.Y += rowDist;
        }
        cur.Y = GetStartOffset(width, rowDist);
        cur.X += columnDist;
    }
}
