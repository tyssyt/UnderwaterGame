// Fill out your copyright notice in the Description page of Project Settings.

#include "IndoorBuilderMode.h"

#include "BuilderModeExtension.h"
#include "The.h"
#include "XD/PlayerControllerX.h"

UIndoorBuilderMode::UIndoorBuilderMode() {
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> HighlightMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/BuilderMode_NotBuildable"));
    HighlightMaterial = HighlightMaterialFinder.Object;
}

UIndoorBuilderMode* UIndoorBuilderMode::Init(UConstructionPlan* constructionPlan) {
    PreInit();
    ConstructionPlan = constructionPlan;
    Preview = GetWorld()->SpawnActor<AIndoorBuilding>(constructionPlan->BuildingClass)->Init(constructionPlan);
    Preview->SetActorTickEnabled(false);
    
    const auto playerController = The::PlayerController(this);
    ConstructionUI->Init(
        constructionPlan,
        The::ConstructionManager(this)
    );
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), ConstructionUI);

    if (Preview->GetBuilderModeExtension())
        Extensions.Add(NewObject<UBuilderModeExtension>(this, Preview->GetBuilderModeExtension()));

    TInlineComponentArray<UComponentX*> components;
    Preview->GetComponents<UComponentX>(components);
    for (const auto component : components)
        if (component->GetBuilderModeExtension())
            Extensions.Add(NewObject<UBuilderModeExtension>(this, component->GetBuilderModeExtension()));

    for (UBuilderModeExtension* extension : Extensions)
        extension->Init(Preview, ConstructionUI);
    

    // bind keys
    const auto inputComponent = playerController->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UIndoorBuilderMode::ConfirmPosition);
    return this;
}

bool UIndoorBuilderMode::Tick(const ACameraPawn& camera) {
    if (!Preview)
        return true;

    Position(camera);

    ConstructionUI->ConstructionMaterials->UpdateHave(The::ConstructionManager(this));
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

    const auto options = NewObject<UConstructionOptions>();
    Stop(options);

    Preview->Habitat->PlaceBuilding(Preview);
    const auto constructionSite = NewObject<UConstructionSite>()->Init(Preview, ConstructionPlan, options);
    The::ConstructionManager(this)->AddConstruction(constructionSite);
    Preview = nullptr;
}

void UIndoorBuilderMode::Stop(UConstructionOptions* options) {
    if (!Preview)
        return;
    
    for (const auto extension : Extensions)
        extension->End(options);
    Extensions.Empty();

    if (!options) {
        Preview->Destroy();
        Preview = nullptr;
    }

    const auto playerController = The::PlayerController(this);
    playerController->Deselect();

    // undo all input bindings
    const TObjectPtr<UInputComponent> inputComponent = playerController->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);
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