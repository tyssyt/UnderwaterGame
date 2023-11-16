// Fill out your copyright notice in the Description page of Project Settings.

#include "AssemblyLine.h"

AAssemblyLine::AAssemblyLine() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/AssemblyLine"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
}