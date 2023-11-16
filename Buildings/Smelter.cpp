// Fill out your copyright notice in the Description page of Project Settings.

#include "Smelter.h"

ASmelter::ASmelter() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Smelter/Smelter"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
}
