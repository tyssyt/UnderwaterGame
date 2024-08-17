// Fill out your copyright notice in the Description page of Project Settings.

#include "IndoorBuilderMode.h"

#include "BuilderModeExtension.h"
#include "ConstructionSite.h"
#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Habitat.h"

UIndoorBuilderMode* UIndoorBuilderMode::Init(UConstructionPlan* constructionPlan) {
    PreInit();
    ConstructionPlan = constructionPlan;
    Preview = Cast<AIndoorBuilding>(ABuilding::Spawn(GetWorld(), constructionPlan));
    Preview->AddCondition(NewObject<UInBuilderMode>(this)->WithSource(this));
    
    const auto playerController = The::PlayerController(this);
    ConstructionUI->Init(
        constructionPlan,
        The::ConstructionManager(this)
    );
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), ConstructionUI);

    Extensions = Preview->CreateBuilderModeExtension();
    if (const auto extension = Extensions->BuildingExtension)
        extension->Init(Preview, ConstructionUI);
    for (const auto& extension : Extensions->ComponentExtensions)
        extension.Value->Init(Preview, ConstructionUI);

    // bind keys
    const auto inputComponent = playerController->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UIndoorBuilderMode::ConfirmPosition);
    inputComponent->BindAction("RotateBlueprint90", IE_Pressed, this, &UIndoorBuilderMode::Rotate);

    return this;
}

bool UIndoorBuilderMode::Tick(const ACameraPawn& camera) {
    if (!Preview)
        return true;

    Position(camera);

    ConstructionUI->ConstructionMaterials->UpdateHave(The::ConstructionManager(this));

    if (const auto extension = Extensions->BuildingExtension)
        extension->Update();
    for (const auto& extension : Extensions->ComponentExtensions)
        extension.Value->Update();

    return false;
}

void UIndoorBuilderMode::Position(const ACameraPawn& camera) {    
    Preview->SetActorLocation(camera.GetActorLocation());

    // project mouse
    FHitResult hitResult;
    const auto playerController = The::PlayerController(this);
    if (!playerController->bShowMouseCursor || !playerController->GetHitResultUnderCursor(ECC_Visibility, true, hitResult)) {
        SetInvisible();
        return;
    }

    Preview->SetActorHiddenInGame(false);
    Preview->SetActorLocation(hitResult.ImpactPoint);
    Preview->Habitat = nullptr;

    // check if mouse is over habitat
    const auto habitat = Cast<AHabitat>(hitResult.GetActor());
    if (!habitat) {
        SetNotBuildable();
        return;
    }

    // get build grid coordinates
    const auto gridPos = habitat->FindCoordinates(hitResult.ImpactPoint);
    if (!gridPos.Key) {
        SetNotBuildable();
        return;
    }
    const int x = gridPos.Value.Key;
    const int y = gridPos.Value.Value;
    Preview->SetCoordinates(x, y, habitat);

    if (!habitat->CanPlaceBuilding(Preview)) {
        SetNotBuildable();
        return;
    }

    SetBuildable();
}

void UIndoorBuilderMode::ConfirmPosition() {
    if (!Buildable)
        return;

    Stop(false);
    Preview->RemoveConditions(this);
    Preview->Habitat->PlaceBuilding(Preview);
    NewObject<UConstructionSite>(Preview)->Init(Preview, ConstructionPlan, Extensions)->QueueTasks();
    Preview = nullptr;
}

void UIndoorBuilderMode::Stop(bool cancelled) {
    if (!Preview)
        return;

    if (const auto extension = Extensions->BuildingExtension)
        extension->End(cancelled);
    for (const auto& extension : Extensions->ComponentExtensions)
        extension.Value->End(cancelled);

    if (cancelled) {
        Preview->Destroy();
        Preview = nullptr;
    }

    const auto playerController = The::PlayerController(this);
    playerController->Deselect();

    // undo all input bindings
    const TObjectPtr<UInputComponent> inputComponent = playerController->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);
    inputComponent->RemoveActionBinding("RotateBlueprint90", IE_Pressed);
}

UClass* UIndoorBuilderMode::IDK() {
    return ConstructionPlan->BuildingClass;
}

void UIndoorBuilderMode::SetInvisible() {
    Buildable = false;
    Preview->SetActorHiddenInGame(true);
}

void UIndoorBuilderMode::SetNotBuildable() {
    Buildable = false;
    Preview->SetActorHiddenInGame(false);
    Preview->AddCondition(HighlightedInvalid);
}

void UIndoorBuilderMode::SetBuildable() {
    Buildable = true;
    Preview->SetActorHiddenInGame(false);
    Preview->RemoveCondition(HighlightedInvalid);
}

void UIndoorBuilderMode::Rotate() {
    Preview->Rotate();
}
