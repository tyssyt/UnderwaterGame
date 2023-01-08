// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "PlayerControllerX.h"
#include "BuilderMode.h"
#include "GameInstanceX.h"

#include "Buildings/WorkerHouse.h"
#include "Buildings/Habitat.h"
#include "Buildings/Solar.h"
#include "Buildings/Substation.h"

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

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UMaterial> RedGhostMaterial;
        FConstructorStatics() : RedGhostMaterial(TEXT("/Game/RedGhostMaterial")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    RedGhostMaterial = ConstructorStatics.RedGhostMaterial.Get();
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay() {
    Super::BeginPlay();
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //TODO scale isn't working in some places, fix!
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::S, -1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Up, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Down, -1.f));
    PlayerInputComponent->BindAxis("MoveForward", this, &ACameraPawn::MoveForward);

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::A, -1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::D, 1.f));
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
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController)
        playerController->TickUI();

    if (BuilderMode)
        BuilderMode->Tick(*this);
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


bool ACameraPawn::IsShowMouseCursor() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    return playerController && playerController->bShowMouseCursor;
}

void ACameraPawn::ShowMouseCursorTrue() {
    ShowMouseCursor(true);
}

void ACameraPawn::ShowMouseCursorFalse() {
    ShowMouseCursor(false);
}

void ACameraPawn::ShowMouseCursor(bool showMouseCursor) {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController)
        playerController->ShowMouseCursor(showMouseCursor);
}

void ACameraPawn::SelectUnderCursor() {

    if (BuilderMode) {
        AActor* building = BuilderMode->Confirm(*this);
        if (building) {
            delete BuilderMode;
            BuilderMode = nullptr;
            GetWorld()->GetGameInstance<UGameInstanceX>()->TheConstructionManager->AddConstruction(building);
            //GetController<APlayerControllerX>()->UpdateSelected(building);
        }
        return;
    }

    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController) {
        AActor* underCursor = playerController->GetUnderCursor<AActor>();
        if (underCursor)
            playerController->UpdateSelected(underCursor);
    }

    /*

    if (playerController && playerController->bShowMouseCursor) {
        float mouseX, mouseY;
        FHitResult hitResult;
        if (playerController->GetMousePosition(mouseX, mouseY) && playerController->GetHitResultAtScreenPosition(FVector2D(mouseX, mouseY), ECollisionChannel::ECC_Visibility, false, hitResult)) {
            if (hitResult.Actor.IsValid()) {
                playerController->UpdateSelected(hitResult.Actor.Get());
            }
        }
    }

    */
}

void ACameraPawn::Deselect() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (playerController) {
        playerController->Deselect();
    }

    if (BuilderMode) {
        delete BuilderMode;
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
            delete BuilderMode; // cancel existing Builder Mode
    }

    BuilderMode = new BuildingBuilderMode(GetWorld()->SpawnActor<ADepot>(GetActorLocation(), FRotator::ZeroRotator));
}

void ACameraPawn::Hotbar2() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == ASmelter::StaticClass())
            return; // right Builder Mode is already active
        else
            delete BuilderMode; // cancel existing Builder Mode
    }

    auto smelter = GetWorld()->SpawnActor<ASmelter>(GetActorLocation(), FRotator::ZeroRotator);
    BuilderMode = new BuildingBuilderMode(smelter);
}

void ACameraPawn::Hotbar3() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AConveyor::StaticClass())
            return; // right Builder Mode is already active
        else
            delete BuilderMode; // cancel existing Builder Mode
    }
    BuilderMode = new ConveyorBuilderMode();
}

void ACameraPawn::Hotbar4() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AHabitat::StaticClass())
            return; // right Builder Mode is already active
        else
            delete BuilderMode; // cancel existing Builder Mode
    }

    auto habitat = GetWorld()->SpawnActor<AHabitat>(GetActorLocation(), FRotator::ZeroRotator);
    BuilderMode = new BuildingBuilderMode(habitat);
}

void ACameraPawn::Hotbar5() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == AWorkerHouse::StaticClass())
            return; // right Builder Mode is already active
        else
            delete BuilderMode; // cancel existing Builder Mode
    }

    auto house = GetWorld()->SpawnActor<AWorkerHouse>(GetActorLocation(), FRotator::ZeroRotator);
    BuilderMode = new IndoorBuilderMode(house);
}

void ACameraPawn::Hotbar6() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == ASolar::StaticClass())
            return; // right Builder Mode is already active
        else
            delete BuilderMode; // cancel existing Builder Mode
    }

    auto solar = GetWorld()->SpawnActor<ASolar>(GetActorLocation(), FRotator::ZeroRotator);
    BuilderMode = new BuildingBuilderMode(solar);
}

void ACameraPawn::Hotbar7() {
    APlayerControllerX* playerController = GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor)
        return;

    if (BuilderMode) {
        if (BuilderMode->IDK() == ASubstation::StaticClass())
            return; // right Builder Mode is already active
        else
            delete BuilderMode; // cancel existing Builder Mode
    }

    auto substation = GetWorld()->SpawnActor<ASubstation>(GetActorLocation(), FRotator::ZeroRotator);
    BuilderMode = new BuildingBuilderMode(substation);
}

void ACameraPawn::Hotbar8() {}
void ACameraPawn::Hotbar9() {}
void ACameraPawn::Hotbar0() {}
