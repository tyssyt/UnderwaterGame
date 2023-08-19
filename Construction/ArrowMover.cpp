// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowMover.h"

#include "Kismet/KismetMathLibrary.h"
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

    const APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    // check if left mouse button is pressed, otherwise stop ticking
    if (!playerController || !playerController->bShowMouseCursor || !playerController->IsInputKeyDown(EKeys::LeftMouseButton)) {
        SetComponentTickEnabled(false);
        Lowlight();
        EndMove();
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
    const APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    
    // Get Mouse in World
    FVector mouseOrigin, mouseDir;
    if (!playerController->DeprojectMousePositionToWorld(mouseOrigin, mouseDir)) {
        return;
    }

    if (FMath::IsNearlyZero(mouseDir.Z))
        return;

    const FVector arrowLocation = GetComponentLocation();
    
    // Intersect mouse with z-Plane of arrow
    const double distance = (arrowLocation.Z - mouseOrigin.Z) / mouseDir.Z;
    FVector intersect = mouseOrigin + mouseDir * distance;
    intersect.Z = arrowLocation.Z; // fix rounding


    // Project mouse onto Arrow Line
    FVector projection = (intersect - arrowLocation).ProjectOnToNormal(GetComponentRotation().Vector()) + arrowLocation;
    projection.Z = arrowLocation.Z; // fix rounding
    

    if (FMath::IsNaN(MouseOffset.X)) {
        // On first call to move, compute Offset
        MouseOffset = projection - arrowLocation;
        return;
    }
    
    // Move Arrow to Projected Mouse Pos
    GetAttachParent()->AddWorldOffset(projection - (arrowLocation + MouseOffset));
    OnArrowMoved.ExecuteIfBound();
}

void UArrowMoverLine::EndMove() {
    MouseOffset.X = NAN;
}


UArrowMoverUp::UArrowMoverUp() {
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/Arrow"));
    UStaticMeshComponent::SetStaticMesh(PlaneMesh.Get());
    SetRelativeRotation(FRotator(90, 0, 0));
}

void UArrowMoverUp::Move() {
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
        FCollisionQueryParams queryParams;
        queryParams.bTraceComplex = true;
        FHitResult hitResult;
        if (!GetWorld()->LineTraceSingleByChannel(hitResult, arrowLocation, arrowLocation + FVector(0., 0., -1.) * playerController->HitResultTraceDistance, COLLISION_LANDSCAPE, queryParams))
            return;

        // keep z-Offset in sync with UConveyorBuilderMode 
        MinZ = hitResult.ImpactPoint.Z + 15.;
    }

    const double intersectZ = FMath::LinePlaneIntersection(mouseOrigin, mouseOrigin + mouseDir, arrowLocation, FVector(mouseDir.X, mouseDir.Y, 0)).Z;

    if (FMath::IsNaN(MouseOffset)) {
        // On first call to move, compute Offset
        MouseOffset = intersectZ - arrowLocation.Z;
        return;
    }
    
    const FVector parentLoc = GetAttachParent()->GetComponentLocation();
    const double newParentZ = intersectZ + (parentLoc.Z - arrowLocation.Z - MouseOffset);
    
    double heightOverMinZ = FMath::Max(newParentZ - MinZ, 0.);
    static double HEIGHT_STEPS = 25;
    heightOverMinZ = FMath::RoundHalfFromZero(heightOverMinZ / HEIGHT_STEPS)*HEIGHT_STEPS;
        
    // Move Arrow to Projected Mouse Pos
    GetAttachParent()->SetWorldLocation(FVector(parentLoc.X, parentLoc.Y, MinZ + heightOverMinZ));

    OnArrowMoved.ExecuteIfBound();
}

void UArrowMoverUp::EndMove() {
    MouseOffset = NAN;
}

UArrowMoverRotate::UArrowMoverRotate() {    
    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh(TEXT("/Game/Assets/Meshes/RoundArrow"));
    UStaticMeshComponent::SetStaticMesh(PlaneMesh.Get());
}

void UArrowMoverRotate::Move() {
    const APlayerController* playerController = GetWorld()->GetFirstPlayerController();
    
    // Get Mouse in World
    FVector mouseOrigin, mouseDir;
    if (!playerController->DeprojectMousePositionToWorld(mouseOrigin, mouseDir)) {
        return;
    }

    if (FMath::IsNearlyZero(mouseDir.Z))
        return;

    const FVector arrowLocation = GetComponentLocation();
    
    // Intersect mouse with z-Plane of arrow
    const double distance = (arrowLocation.Z - mouseOrigin.Z) / mouseDir.Z;
    FVector intersect = mouseOrigin + mouseDir * distance;
    intersect.Z = arrowLocation.Z; // fix rounding

    // compute angle between mouse and arrow
    const FVector parentLoc = GetAttachParent()->GetComponentLocation();
    const FVector centerToMouse = parentLoc - intersect;
    const FVector centerToArrow = parentLoc - arrowLocation;    
    const double angle = FMath::Atan2(centerToMouse.Y, centerToMouse.X) - FMath::Atan2(centerToArrow.Y, centerToArrow.X);    
    if (!FMath::IsFinite(angle))
        return;

    if (FMath::IsNaN(MouseOffset)) {
        // On first call to move, compute Offset
        MouseOffset = angle;
        return;
    }

    // rotate by angle
    GetAttachParent()->AddWorldRotation(FRotator(0., FMath::RadiansToDegrees(angle - MouseOffset), 0.));
    
    OnArrowMoved.ExecuteIfBound();
}

void UArrowMoverRotate::EndMove() {
    MouseOffset = NAN;
}
