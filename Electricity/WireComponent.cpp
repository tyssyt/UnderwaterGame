// Fill out your copyright notice in the Description page of Project Settings.

#include "WireComponent.h"

#include "XD/Buildings/Building.h"

UWireComponent::UWireComponent() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Wire"));
    UStaticMeshComponent::SetStaticMesh(MeshFinder.Object);

    SetForwardAxis(ESplineMeshAxis::X, false);
    SetStartTangent(FVector(0., 0., 360.), false);
    SetEndTangent(FVector(0., 0., -360.), false);
}

UWireComponent* UWireComponent::Create(AActor* wireParent, ABuilding* start, ABuilding* end) {
    // TODO just like in the other places where I create components, figure out what of this is actually needed
    UWireComponent* wire = NewObject<UWireComponent>(wireParent);
    USceneComponent* root = wireParent->GetRootComponent();
    wire->SetMobility(root->Mobility);
    wire->RegisterComponent();
    wire->AttachToComponent(root, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    wireParent->AddInstanceComponent(wire);

    wire->SetStart(start, false);
    wire->SetEnd(end, true);

    return wire;
}

void UWireComponent::SetStart(ABuilding* start, const bool bUpdateMesh) {
    const AActor* owner = GetOwner();
    SetStartPosition(owner->GetActorRotation().UnrotateVector(start->GetActorLocation() - owner->GetActorLocation()), bUpdateMesh);
    Start = start;
}
void UWireComponent::SetEnd(ABuilding* end, const bool bUpdateMesh) {
    const AActor* owner = GetOwner();
    SetEndPosition(owner->GetActorRotation().UnrotateVector(end->GetActorLocation() - owner->GetActorLocation()), bUpdateMesh);
    End = end;
}
