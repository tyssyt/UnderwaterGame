// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowMover.h"

#include "XD/PlayerControllerX.h"


UArrowMover::UArrowMover() {
    PrimaryComponentTick.bCanEverTick = true;
}

void UArrowMover::BeginPlay() {
    Super::BeginPlay();

    OnClicked.AddDynamic(this, &UArrowMover::OnClick);
    OnBeginCursorOver.AddDynamic(this, &UArrowMover::OnHoverBegin);
    OnEndCursorOver.AddDynamic(this, &UArrowMover::OnHoverEnd);

    SetGenerateOverlapEvents(false);
    SetComponentTickEnabled(false);
}

void UArrowMover::OnClick(UPrimitiveComponent*, FKey) {
    SetComponentTickEnabled(true);
    Highlight();
}

void UArrowMover::OnHoverBegin(UPrimitiveComponent*) {
    HasHover = true;
    Highlight();
}

void UArrowMover::OnHoverEnd(UPrimitiveComponent*) {
    HasHover = false;
    Lowlight();
}

const static FName ParameterNameOpacity = FName(TEXT("Opacity Amount"));

void UArrowMover::Highlight() {
    UMaterialInterface* MaterialInterface = GetMaterial(0);
    UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(MaterialInterface);
    if (!MaterialInstance) {
        MaterialInstance = UMaterialInstanceDynamic::Create(MaterialInterface, this);
        Super::SetMaterial(0, MaterialInstance);
    }

    MaterialInstance->SetScalarParameterValue(ParameterNameOpacity, 1.);
}

void UArrowMover::Lowlight() const {
    if (HasHover || IsComponentTickEnabled())
        return;

    UMaterialInstanceDynamic* MaterialInstance = Cast<UMaterialInstanceDynamic>(GetMaterial(0));
    if (!MaterialInstance)
        return;
    MaterialInstance->SetScalarParameterValue(ParameterNameOpacity, 0.7);
}

void UArrowMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    // check if left mouse button is pressed, otherwise stop ticking
    if (!playerController || !playerController->bShowMouseCursor || !playerController->IsInputKeyDown(EKeys::LeftMouseButton)) {
        SetComponentTickEnabled(false);
        Lowlight();
        return;
    }

    Move();
}

void UArrowMover::SetMaterial(int32 ElementIndex, UMaterialInterface* Material) {
    // do not set material from external calls
}

UArrowMoverLine::UArrowMoverLine() {
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/Arrow"));
    UStaticMeshComponent::SetStaticMesh(PlaneMesh.Get());
}

void UArrowMoverLine::Move() {
    /* TODO
     * Currently, this always centers the model such that the middle of the arrow is under the Mouse.
     * That means if we don't click in the exact center of the arrow, everything will shift right at the beginning of the move.
     * Instead, on the start of the move we should remember the offset of the initial click and use that when moving the arrow.
     */
    
    const APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    
    // Get Mouse in World
    FVector mouseOrigin, mouseDir;
    if (!playerController->DeprojectMousePositionToWorld(mouseOrigin, mouseDir)) {
        return;
    }

    // Intersect mouse with z-Plane of arrow
    if (FMath::IsNearlyZero(mouseDir.Z))
        return;
    FVector arrowLocation = GetComponentLocation();
    double distance = (arrowLocation.Z - mouseOrigin.Z) / mouseDir.Z;
    FVector intersect = mouseOrigin + mouseDir * distance;
    intersect.Z = arrowLocation.Z; // fix rounding


    // Project mouse onto Arrow Line
    FVector projection = (intersect - arrowLocation).ProjectOnToNormal(GetComponentRotation().Vector()) + arrowLocation;
    projection.Z = arrowLocation.Z; // fix rounding
    
    // Move Arrow to Projected Mouse Pos
    GetAttachParent()->AddWorldOffset(projection - arrowLocation);

    OnArrowMoved.ExecuteIfBound();
}


UArrowMoverUp::UArrowMoverUp() {
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/Arrow"));
    UStaticMeshComponent::SetStaticMesh(PlaneMesh.Get());
    SetRelativeRotation(FRotator(90, 0, 0));
}

void UArrowMoverUp::BeginPlay() {
    Super::BeginPlay();
}

void UArrowMoverUp::Move() {
    /* TODO
     * Currently, this always centers the model such that the middle of the arrow is under the Mouse.
     * That means if we don't click in the exact center of the arrow, everything will shift right at the beginning of the move.
     * Instead, on the start of the move we should remember the offset of the initial click and use that when moving the arrow.
     */    

    const APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    
    // Get Mouse in World
    FVector mouseOrigin, mouseDir;
    if (!playerController->DeprojectMousePositionToWorld(mouseOrigin, mouseDir)) {
        return;
    }

    if (FMath::IsNearlyZero(mouseDir.X) && FMath::IsNearlyZero(mouseDir.Y))
        return;

    const FVector arrowLocation = GetComponentLocation();
    
    if (FMath::IsNaN(MinZ)) {
        // find the height of the landscape below this
        FHitResult hitResult;
        FCollisionQueryParams queryParams;
        queryParams.bTraceComplex = true;
        if (!GetWorld()->LineTraceSingleByChannel(hitResult, arrowLocation, arrowLocation + FVector(0., 0., -1.) * playerController->HitResultTraceDistance, COLLISION_LANDSCAPE, queryParams))
            return;

        // keep z-Offset in sync with UConveyorBuilderMode 
        MinZ = hitResult.ImpactPoint.Z + 15.;
    }

    const double intersectZ = FMath::LinePlaneIntersection(mouseOrigin, mouseOrigin + mouseDir, arrowLocation, FVector(mouseDir.X, mouseDir.Y, 0)).Z;

    const FVector parentLoc = GetAttachParent()->GetComponentLocation();
    const double newParentZ = intersectZ + (parentLoc.Z - arrowLocation.Z);
    
    double heightOverMinZ = FMath::Max(newParentZ - MinZ, 0.);
    static double HEIGHT_STEPS = 25;
    heightOverMinZ = FMath::RoundHalfFromZero(heightOverMinZ / HEIGHT_STEPS)*HEIGHT_STEPS;
        
    // Move Arrow to Projected Mouse Pos
    GetAttachParent()->SetWorldLocation(FVector(parentLoc.X, parentLoc.Y, MinZ + heightOverMinZ));

    OnArrowMoved.ExecuteIfBound();
}

UArrowMoverRotate::UArrowMoverRotate() {    
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/RoundArrow"));
    UStaticMeshComponent::SetStaticMesh(PlaneMesh.Get());
}

void UArrowMoverRotate::Move() {
    /* TODO
     * The Movement is non intuitive. We move in such a way that the Building faces the mouse, however this means that the mouse
     * does not "Grab" the hovering arrow, which is what most people would expect, especially since thats the way the Line mover works
     */
    const APlayerController* playerController = GetWorld()->GetFirstPlayerController();

    // project building into screenspace
    float mouseX, mouseY;
    FVector2D screenLocation;
    if (!playerController->GetMousePosition(mouseX, mouseY) || !playerController->ProjectWorldLocationToScreen(GetAttachmentRoot()->GetComponentLocation(), screenLocation))
        return;

    // X and Y are swapped from Screen to World Space, and the sign is also not right, so the order of terms below gets quite confusing
    const FVector buildingToMouse = -FVector(mouseY - screenLocation.Y, screenLocation.X - mouseX, 0.f);
    FRotator buildingToMouseRot = buildingToMouse.Rotation(); // TODO check if there is a 2D variant of this, should not be to hard
    buildingToMouseRot.Yaw += playerController->PlayerCameraManager->GetCameraRotation().Yaw;
    GetAttachParent()->SetWorldRotation(buildingToMouseRot);
}
