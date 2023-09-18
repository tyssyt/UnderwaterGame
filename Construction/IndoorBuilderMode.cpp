// Fill out your copyright notice in the Description page of Project Settings.

#include "IndoorBuilderMode.h"

#include "BuilderModeExtension.h"
#include "ConstructionSite.h"
#include "XD/GameInstanceX.h"
#include "XD/PlayerControllerX.h"

UIndoorBuilderMode::UIndoorBuilderMode() {
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> HighlightMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/BuilderMode_NotBuildable"));
    HighlightMaterial = HighlightMaterialFinder.Object;
}

UIndoorBuilderMode* UIndoorBuilderMode::Init(UConstructionPlan* constructionPlan, UBuilderModeExtension* builderModeExtension) {
    ConstructionPlan = constructionPlan;
    Preview = GetWorld()->SpawnActor<AIndoorBuilding>(constructionPlan->BuildingClass);
    Preview->SetActorTickEnabled(false);
    
    const APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();
    playerController->BlueprintHolder->ConstructionUI->Set(
        constructionPlan,
        GetWorld()->GetGameInstance<UGameInstanceX>()->TheConstructionManager
    );
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), playerController->BlueprintHolder->ConstructionUI);

    if (builderModeExtension)
        Extensions.Add(builderModeExtension);
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

    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    playerController->BlueprintHolder->ConstructionUI->UpdateHave(GetWorld()->GetGameInstance<UGameInstanceX>()->TheConstructionManager);
    for (UBuilderModeExtension* extension : Extensions)
        extension->Update();

    return false;
}

void UIndoorBuilderMode::Position(const ACameraPawn& camera) {    
    Preview->SetActorLocation(camera.GetActorLocation());

    // project mouse
    FHitResult hitResult;
    const APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor || !playerController->GetHitResultUnderCursor(ECC_Visibility, true, hitResult)) {
        SetInvisible();
        return;
    }

    Preview->SetActorHiddenInGame(false);
    Preview->SetActorLocation(hitResult.ImpactPoint);

    // check if mouse is over habitat
    AHabitat* habitat = Cast<AHabitat>(hitResult.GetActor());
    if (!habitat) {
        SetNotBuildable();
        return;
    }

    // get build grid coordinates
    const auto gridPos = habitat->findCoordinates(hitResult.ImpactPoint);
    if (!gridPos.first) {
        SetNotBuildable();
        return;
    }
    const int x = gridPos.second.first;
    const int y = gridPos.second.second;
    Preview->setCoordinates(x, y, habitat);

    if (!habitat->canPlaceBuilding(Preview)) {
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
        Preview->Habitat->placeBuilding(Preview);
        ConstructionSite* constructionSite = new ConstructionSite(Preview, ConstructionPlan, FConstructionFlags{false});
        GetWorld()->GetGameInstance<UGameInstanceX>()->TheConstructionManager->AddConstruction(constructionSite);
    } else
        Preview->Destroy();

    APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();
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