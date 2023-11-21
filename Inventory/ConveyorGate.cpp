// Fill out your copyright notice in the Description page of Project Settings.

#include "ConveyorGate.h"

UConveyorGate::UConveyorGate() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/ConveyorGate"));
    UStaticMeshComponent::SetStaticMesh(MeshFinder.Object);
}
