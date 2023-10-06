// Fill out your copyright notice in the Description page of Project Settings.

#include "IndoorBuilderMode.h"

#include "BuilderModeExtension.h"
#include "ConstructionSite.h"
#include "XD/GameInstanceX.h"
#include "XD/PlayerControllerX.h"
#include "XD/Utils.h"

UIndoorBuilderMode::UIndoorBuilderMode() {
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> HighlightMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/BuilderMode_NotBuildable"));
    HighlightMaterial = HighlightMaterialFinder.Object;
}

UIndoorBuilderMode* UIndoorBuilderMode::Init(UConstructionPlan* constructionPlan) {
    ConstructionPlan = constructionPlan;
    Preview = GetWorld()->SpawnActor<AIndoorBuilding>(constructionPlan->BuildingClass);
    Preview->SetActorTickEnabled(false);
    
    const auto playerController = The::PlayerController(this);
    playerController->BlueprintHolder->ConstructionUI->Set(
        constructionPlan,
        The::ConstructionManager(this)
    );
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), playerController->BlueprintHolder->ConstructionUI);

    if (constructionPlan->BuilderModeExtension)
        Extensions.Add(NewObject<UBuilderModeExtension>(this, constructionPlan->BuilderModeExtension));
    if (auto elec = Preview->FindComponentByClass<UElectricComponent>())
        Extensions.Add(NewObject<UIndoorElectricityBuilderModeExtension>(this));

    for (UBuilderModeExtension* extension : Extensions)
        extension->Init(Preview);
    

    // bind keys
    const auto inputComponent = playerController->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UIndoorBuilderMode::ConfirmPosition);
    return this;
}

bool UIndoorBuilderMode::Tick(const ACameraPawn& camera) {
    if (!Preview)
        return true;

    Position(camera);

    The::BPHolder(this)->ConstructionUI->UpdateHave(The::ConstructionManager(this));
    for (UBuilderModeExtension* extension : Extensions)
        extension->Update();

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

    Stop(true);
}


void UIndoorBuilderMode::Stop(bool success) {
    if (!Preview)
        return;

    if (success) {
        Preview->Habitat->PlaceBuilding(Preview);
        ConstructionSite* constructionSite = new ConstructionSite(Preview, ConstructionPlan, FConstructionFlags{false});
        The::ConstructionManager(this)->AddConstruction(constructionSite);
    } else
        Preview->Destroy();

    const auto playerController = The::PlayerController(this);
    playerController->Deselect();

    // undo all input bindings
    const TObjectPtr<UInputComponent> inputComponent = playerController->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);

    Preview = nullptr;
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
    Preview->SetAllMaterials(HighlightMaterial);
}

void UIndoorBuilderMode::SetBuildable() {
    Buildable = true;
    Preview->SetActorHiddenInGame(false);
    Preview->SetAllMaterials(nullptr);
}