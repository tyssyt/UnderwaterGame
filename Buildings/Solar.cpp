// Fill out your copyright notice in the Description page of Project Settings.

#include "Solar.h"

ASolar::ASolar() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Solar"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
}
