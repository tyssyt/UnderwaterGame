// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionSite.h"

#include "ConstructionManager.h"
#include "The.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"

UUnderConstruction::UUnderConstruction() {
    const static ConstructorHelpers::FObjectFinder<UMaterial> ConstructionOverlayFinder(TEXT("/Game/Assets/Materials/GhostMaterials/ConstructionOverlay"));
    OverlayMaterial = UMaterialInstanceDynamic::Create(ConstructionOverlayFinder.Object, this, FName(TEXT("ConstructionOverlay Material")));
    Type = EType::NonInteractable;
}

void UUnderConstruction::SetProgress(float progress) const {   
    const auto mad = Cast<UMaterialInstanceDynamic>(OverlayMaterial);
    mad->SetScalarParameterValue("Progress", progress);
}

UConstructionSite* UConstructionSite::Init(ABuilding* building, const UConstructionPlan* constructionPlan, UBuilderModeExtensions* extensions) {
    return Init(building, constructionPlan->Time, constructionPlan->Materials, extensions);
}

UConstructionSite* UConstructionSite::Init(ABuilding* building, int time, const TArray<Material>& materials, UBuilderModeExtensions* extensions) {
    Building = building;
    Time = time;
    Materials = materials;
    Extensions = extensions;

    Condition = NewObject<UUnderConstruction>(this);
    building->AddCondition(Condition);
    return this;
}

void UConstructionSite::QueueTasks() {
    if (The::Cheats(this)->InstantBuild) {
        FinishConstruction();
        return;
    }

    const auto constructionManager = The::ConstructionManager(this);
    for (const auto& material : Materials) {
        const auto task = NewObject<UDeliverResource>(this)->Init(this, material);
        Tasks.Add(task);
        constructionManager->AddTask(task);
    }
}

void UConstructionSite::Tick(float DeltaTime) {    
    // TODO if this never explodes, we can remove it at some point
    if (LastFrameNumberWeTicked == GFrameCounter) {
        checkNoEntry();
        return;
    }
    LastFrameNumberWeTicked = GFrameCounter;

    if (State != EState::Constructing)
        return;

    Progress++;
    Condition->SetProgress(static_cast<float>(Progress)/Time);
    if (Progress >= Time)
        FinishConstruction();
}

void UConstructionSite::StartConstruction() {
    State = EState::Constructing;
}

void UConstructionSite::FinishConstruction() {
    State = EState::Finished;
    Building->RemoveCondition(Condition);
    Building->OnConstructionComplete(Extensions);
}

void UDeliverResource::PickupMaterial() const {
    const auto input = PickupFrom->Inventory->GetInput(RequiredMaterial.resource);
    check(input && input->Current >= RequiredMaterial.amount);
    input->PullFrom(RequiredMaterial.amount);
    PickupFrom->Unreserve(RequiredMaterial);
}

ABuilderShip::FCommand UDeliverResource::Deliver() const {
    FVector target;
    if (const auto indoorBuilding = Cast<AIndoorBuilding>(ConstructionSite->Building))
        target = indoorBuilding->Habitat->GetActorLocation();
    else
        target = ConstructionSite->Building->GetActorLocation();
    return ABuilderShip::FCommand(target);
}
ABuilderShip::FCommand UDeliverResource::Construct() {
    ConstructionSite->StartConstruction();
    if (const auto indoorBuilding = Cast<AIndoorBuilding>(ConstructionSite->Building)) {
        return ABuilderShip::FCommand();
    }
    return ABuilderShip::FCommand(ConstructionSite->Time);
}

UDeliverResource* UDeliverResource::Init(UConstructionSite* constructionSite, const Material& material) {
    ConstructionSite = constructionSite;
    if (const auto indoorBuilding = Cast<AIndoorBuilding>(constructionSite->Building))
        Location = indoorBuilding->Habitat->GetActorLocation();
    else
        Location = constructionSite->Building->GetActorLocation();
    RequiredMaterial = material;
    return this;
}

ABuilderShip::FCommand UDeliverResource::GetNextCommand() {
    switch (NextCommand) {
    case EState::Collect:
        NextCommand = EState::Deliver;
        return ABuilderShip::FCommand(PickupFrom->GetActorLocation());
    case EState::Deliver:
        NextCommand = EState::Construct;
        PickupMaterial();
        return Deliver();
    case EState::Construct:
        NextCommand = EState::Done;
        ConstructionSite->Tasks.Remove(this);
        if (ConstructionSite->Tasks.IsEmpty())
            return Construct();
        return ABuilderShip::FCommand();
    case EState::Done:
        return ABuilderShip::FCommand();
    }
    checkNoEntry();
    return ABuilderShip::FCommand();
}
