// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "PlayerControllerX.h"
#include "Construction/BuilderMode.h"
#include "GameInstanceX.h"

#include "Buildings/WorkerHouse.h"
#include "Buildings/Habitat.h"
#include "Buildings/Solar.h"
#include "Buildings/Substation.h"
#include "Buildings/PickupPad.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerInput.h"

// Sets default values
ACameraPawn::ACameraPawn() {
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = RootComponent;
    //MovementComponent->SetPlaneConstraintEnabled(true);
    //MovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z); with this enabled, zooming in/out doesn't work.

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay() {
    Super::BeginPlay();
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //TODO scale isn't working in some places, fix!
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Up, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Down, -1.f));
    PlayerInputComponent->BindAxis("MoveForward", this, &ACameraPawn::MoveForward);

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::Left, -1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::Right, 1.f));
    PlayerInputComponent->BindAxis("MoveRight", this, &ACameraPawn::MoveRight);

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Add, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Subtract, -1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::MouseWheelAxis, 1.f));
    PlayerInputComponent->BindAxis("MoveUp", this, &ACameraPawn::MoveUp);

    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ShowMouseCursor", EKeys::LeftAlt));
    PlayerInputComponent->BindAction("ShowMouseCursor", EInputEvent::IE_Pressed, this, &ACameraPawn::ShowMouseCursorFalse);
    PlayerInputComponent->BindAction("ShowMouseCursor", EInputEvent::IE_Released, this, &ACameraPawn::ShowMouseCursorTrue);

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Turn", EKeys::MouseX, 1.f));
    PlayerInputComponent->BindAxis("Turn", this, &ACameraPawn::Turn);

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookUp", EKeys::MouseY, 1.f));
    PlayerInputComponent->BindAxis("LookUp", this, &ACameraPawn::LookUp);

    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Select", EKeys::LeftMouseButton));
    PlayerInputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &ACameraPawn::SelectUnderCursor);

    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Deselect", EKeys::Escape));
    PlayerInputComponent->BindAction("Deselect", EInputEvent::IE_Pressed, this, &ACameraPawn::Deselect);

    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar1", EKeys::One));
    PlayerInputComponent->BindAction("Hotbar1", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar1);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar2", EKeys::Two));
    PlayerInputComponent->BindAction("Hotbar2", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar2);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar3", EKeys::Three));
    PlayerInputComponent->BindAction("Hotbar3", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar3);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar4", EKeys::Four));
    PlayerInputComponent->BindAction("Hotbar4", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar4);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar5", EKeys::Five));
    PlayerInputComponent->BindAction("Hotbar5", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar5);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar6", EKeys::Six));
    PlayerInputComponent->BindAction("Hotbar6", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar6);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar7", EKeys::Seven));
    PlayerInputComponent->BindAction("Hotbar7", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar7);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar8", EKeys::Eight));
    PlayerInputComponent->BindAction("Hotbar8", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar8);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar9", EKeys::Nine));
    PlayerInputComponent->BindAction("Hotbar9", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar9);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Hotbar0", EKeys::Zero));
    PlayerInputComponent->BindAction("Hotbar0", EInputEvent::IE_Pressed, this, &ACameraPawn::Hotbar0);

    // Actions of BuilderMode
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("PushBlueprintUp", EKeys::W));
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("PushBlueprintDown", EKeys::S));
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("PushBlueprintLeft", EKeys::A));
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("PushBlueprintRight", EKeys::D));
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RotateBlueprintLeft", EKeys::Q));
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RotateBlueprintRight", EKeys::E));
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RotateBlueprint90", EKeys::R));
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController)
        playerController->TickUI();

    if (BuilderMode && BuilderMode->Tick(*this))
        BuilderMode = nullptr;
}

void ACameraPawn::MoveForward(float val) {
    if (val != 0.f) {
        FVector movementDir = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X); // TODO I normalize, so getting a scaled axis is pointless?
        movementDir.Z = 0;
        movementDir.Normalize();
        AddMovementInput(movementDir, val);
    }
}

void ACameraPawn::MoveRight(float val) {
    if (val != 0.f) {
        FVector movementDir = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y); // TODO I normalize, so getting a scaled axis is pointless?
        movementDir.Z = 0;
        movementDir.Normalize();
        AddMovementInput(movementDir, val);
    }
}

void ACameraPawn::MoveUp(float val) {
    if (val != 0.f)
        AddMovementInput(FVector::UpVector, -val); //TODO move negation to scale
}

void ACameraPawn::Turn(float val) {
    AddControllerYawInput(val);
}

void ACameraPawn::LookUp(float val) {
    AddControllerPitchInput(-val); //TODO move negation to scale
}


bool ACameraPawn::IsShowMouseCursor() const {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    return playerController && playerController->bShowMouseCursor;
}

void ACameraPawn::ShowMouseCursorTrue() {
    ShowMouseCursor(true);
}

void ACameraPawn::ShowMouseCursorFalse() {
    ShowMouseCursor(false);
}

void ACameraPawn::ShowMouseCursor(bool showMouseCursor) const {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController)
        playerController->ShowMouseCursor(showMouseCursor);
}

void ACameraPawn::SelectUnderCursor() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController) {
        AActor* underCursor = playerController->GetUnderCursor<AActor>();
        if (underCursor)
            playerController->UpdateSelected(underCursor);
    }
}

void ACameraPawn::Deselect() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController) {
        playerController->Deselect();
    }

    if (BuilderMode) {
        BuilderMode->Stop();
        BuilderMode = nullptr;
    }
}

void ACameraPawn::Hotbar1() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor) {
        return;
    }

    if (BuilderMode) {
        if (BuilderMode->IDK() == ADepot::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop();
    }

    BuilderMode = NewObject<UBuildingBuilderMode>(this)->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->Depot, GetWorld());
}

void ACameraPawn::Hotbar2() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == ASmelter::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UBuildingBuilderMode>(this)->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->Smelter, GetWorld());
}

void ACameraPawn::Hotbar3() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AConveyor::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }
    BuilderMode = NewObject<UConveyorBuilderMode>(this)->Init();
}

void ACameraPawn::Hotbar4() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AHabitat::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UBuildingBuilderMode>()->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->Habitat, GetWorld());
}

void ACameraPawn::Hotbar5() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AWorkerHouse::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UIndoorBuilderMode>()->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->WorkerHouse, GetWorld());
}

void ACameraPawn::Hotbar6() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == ASolar::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UBuildingBuilderMode>()->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->Solar, GetWorld());
}

void ACameraPawn::Hotbar7() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == ASubstation::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UBuildingBuilderMode>()->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->Substation, GetWorld());
}

void ACameraPawn::Hotbar8() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == APickupPad::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UBuildingBuilderMode>()->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->PickupPad, GetWorld());
}

void ACameraPawn::Hotbar9() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AAssemblyLine::StaticClass())
            return; // right Builder Mode is already active
        else
            BuilderMode->Stop(); // TODO we know we can delete here because no one else reference it, but we need to wait for GC because unreal wants us to
    }

    BuilderMode = NewObject<UBuildingBuilderMode>()->Init(GetGameInstance<UGameInstanceX>()->TheBuildingBook->AssemblyLine, GetWorld());    
}
void ACameraPawn::Hotbar0() {}
