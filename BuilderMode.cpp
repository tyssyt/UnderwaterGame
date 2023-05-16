#include "BuilderMode.h"

#include "GameInstanceX.h"
#include "PlayerControllerX.h"
#include "Buildings/Conveyor.h"
#include "Buildings/Habitat.h"

#include "Kismet/GameplayStatics.h"

// BuildingBuilderMode

BuildingBuilderMode::BuildingBuilderMode(UConstructionPlan* constructionPlan, UWorld* world)
        : ConstructionPlan(constructionPlan), Preview(world->SpawnActor<ABuilding>(constructionPlan->BuildingClass)) {
    Preview->SetActorTickEnabled(false);    
}

BuildingBuilderMode::~BuildingBuilderMode() {
    if (Phase != Done)
        Preview->Destroy();
}

void BuildingBuilderMode::Tick(const ACameraPawn& camera) {
    switch (Phase) {
    case Positioning:
        Position(camera);
        break;
    case Rotating:
        Rotate(camera);
        break;
    case Done:
        break;
    }
}

void BuildingBuilderMode::Position(const ACameraPawn& camera) const {
    APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor) {
        Preview->SetActorHiddenInGame(true);
        return;
    }

    // move Preview to Camera
    Preview->SetActorLocation(camera.GetActorLocation());

    // find Vector From Screen along Mouse
    float mouseX, mouseY;
    FVector worldOrigin, worldDirection;
    if (!playerController->GetMousePosition(mouseX, mouseY) || !UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(mouseX, mouseY), worldOrigin, worldDirection)) {
        Preview->SetActorHiddenInGame(true);
        return;
    }

    // move Preview along Vector with collision
    FHitResult hitResult;
    Preview->SetActorLocation(Preview->GetActorLocation() + worldDirection * 10000.f, true, &hitResult, ETeleportType::TeleportPhysics); // TODO is *10000 enough?
    if (!hitResult.IsValidBlockingHit()) {
        Preview->SetActorHiddenInGame(true);
        return;
    }

    // TODO check if Hit Target Is Ground and color Code the Preview
    Preview->SetActorHiddenInGame(false);
}

void BuildingBuilderMode::Rotate(const ACameraPawn& camera) {
    APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    // project building into screenspace
    float mouseX, mouseY;
    FVector2D screenLocation;
    if (!playerController->GetMousePosition(mouseX, mouseY) || !playerController->ProjectWorldLocationToScreen(Preview->GetActorLocation(), screenLocation))
        return;

    // X and Y are swapped from Screen to World Space, and the sign is also not right, so the order of terms below gets quite confusing
    FVector buildingToMouse = FVector(mouseY - screenLocation.Y, screenLocation.X - mouseX, 0.f);
    FRotator buildingToMouseRot = FRotationMatrix::MakeFromX(buildingToMouse).Rotator(); // TODO check if there is a 2D variant of this, should not be to hard
    buildingToMouseRot.Yaw += camera.GetActorRotation().Yaw;
    Preview->SetActorRotation(buildingToMouseRot, ETeleportType::TeleportPhysics);


    //// project mouse into worldspace
    //FHitResult hitResult;
    //if (!playerController->GetUnderCursor(&hitResult))
    //	return;
    //
    //// rotate building to look at mouse
    //FVector buildingToMouse = hitResult.Location - Preview->GetActorLocation();
    //FRotator rotator = FRotationMatrix::MakeFromX(buildingToMouse).Rotator();
    //rotator.Pitch = .0f;
    //Preview->SetActorRotation(rotator, ETeleportType::TeleportPhysics);
}

ConstructionSite* BuildingBuilderMode::Confirm(const ACameraPawn& camera) {
    switch (Phase) {
    case Positioning:
        if (!Preview->IsHidden())
            Phase = Rotating;
        return nullptr;
    case Rotating:
        Phase = Done;
        // TODO remove color coding
        return new ConstructionSite(Preview, ConstructionPlan);
    case Done:
        checkNoEntry();
        return new ConstructionSite(Preview, ConstructionPlan);
    default:
        checkNoEntry();
        return nullptr;
    }
}

UClass* BuildingBuilderMode::IDK() {
    return ConstructionPlan->BuildingClass;
}

// ConveyorBuilderMode

ConveyorBuilderMode::ConveyorBuilderMode() {}

void ConveyorBuilderMode::Tick(const ACameraPawn& camera) {}

ConstructionSite* ConveyorBuilderMode::Confirm(const ACameraPawn& camera) {
    //TODO return logs once we have visual feedback
    APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController)
        return nullptr;

    ABuilding* underCursor = playerController->GetUnderCursor<ABuilding>();
    if (!underCursor)
        return nullptr;

    if (!Source) {
        UInventoryComponent* inventory = underCursor->FindComponentByClass<UInventoryComponent>();
        if (inventory && inventory->GetOutputs().Num()) {
            UE_LOG(LogTemp, Warning, TEXT("Source Selected"));
            Source = underCursor;
        } else {
            UE_LOG(LogTemp, Warning, TEXT("Source does not have an Inventory that can be pulled from."));
        }
        return nullptr;
    }

    UInventoryComponent* inventory = underCursor->FindComponentByClass<UInventoryComponent>();
    if (inventory && inventory->GetInputs().Num()) {
        UE_LOG(LogTemp, Warning, TEXT("Target Selected"));
        
        //TODO do the more complex check in the connect before creating the conveyor, possibly leading to the resource selection dialog
        AConveyor* conveyor = AConveyor::Create(camera.GetWorld(), Source, underCursor, 100); // TODO where should the throughput actually be configured?
        return new ConstructionSite(conveyor, 1, {Material(10, conveyor->GetGameInstance<UGameInstanceX>()->TheResourceBook->LargeParts)}); // TODO make cost & time scale with length
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Target does not have an Inventory that can be pushed into."));
        return nullptr;
    }
}

UClass* ConveyorBuilderMode::IDK() {
    return AConveyor::StaticClass();
}


// IndoorBuilderMode
IndoorBuilderMode::IndoorBuilderMode(UConstructionPlan* constructionPlan, UWorld* world)
        : ConstructionPlan(constructionPlan), Preview(world->SpawnActor<AIndoorBuilding>(constructionPlan->BuildingClass)) {    
    Preview->SetActorTickEnabled(false);
    OldMaterial = Preview->FindComponentByClass<UStaticMeshComponent>()->GetMaterial(0)->GetMaterial();    
}

IndoorBuilderMode::~IndoorBuilderMode() {
    if (Preview)
        Preview->Destroy();
}

void IndoorBuilderMode::Tick(const ACameraPawn& camera) {
    Preview->SetActorLocation(camera.GetActorLocation());

    // project mouse
    FHitResult hitResult;
    APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor || !playerController->GetUnderCursor(&hitResult)) {
        setInvisible();
        return;
    }

    Preview->SetActorHiddenInGame(false);
    Preview->SetActorLocation(hitResult.ImpactPoint);

    // check if mouse is over habitat
    AHabitat* habitat = Cast<AHabitat>(hitResult.GetActor());
    if (!habitat) {
        setNotBuildable(camera.RedGhostMaterial);
        return;
    }

    // get build grid coordinates
    auto gridPos = habitat->findCoordinates(hitResult.ImpactPoint);
    if (!gridPos.first) {
        setNotBuildable(camera.RedGhostMaterial);
        return;
    }
    int x = gridPos.second.first;
    int y = gridPos.second.second;
    Preview->setCoordinates(x, y, habitat);

    if (!habitat->canPlaceBuilding(Preview)) {
        setNotBuildable(camera.RedGhostMaterial);
        return;
    }

    setBuildable();
}

ConstructionSite* IndoorBuilderMode::Confirm(const ACameraPawn& camera) {
    if (!Buildable) {
        return nullptr;
    }
    AIndoorBuilding* building = Preview;
    Preview = nullptr;

    building->Habitat->placeBuilding(building);
    return new ConstructionSite(building, ConstructionPlan);
}

UClass* IndoorBuilderMode::IDK() {
    return ConstructionPlan->BuildingClass;
}

void IndoorBuilderMode::setInvisible() {
    Buildable = false;
    Preview->SetActorHiddenInGame(true);
}

void IndoorBuilderMode::setNotBuildable(UMaterial* redGhostMaterial) {
    Buildable = false;
    Preview->SetActorHiddenInGame(false);
    Preview->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, redGhostMaterial);

}

void IndoorBuilderMode::setBuildable() {
    Buildable = true;
    Preview->SetActorHiddenInGame(false);
    Preview->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, OldMaterial);
}
