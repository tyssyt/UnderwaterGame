// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingBuilderMode.h"

#include "BuilderModeExtension.h"
#include "ConstructionSite.h"
#include "IndoorBuilderMode.h"
#include "The.h"
#include "XD/CollisionProfiles.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Building.h"

UBuildingBuilderMode* UBuildingBuilderMode::Init(UConstructionPlan* constructionPlan) {
    PreInit();
    ConstructionPlan = constructionPlan;
    Preview = GetWorld()->SpawnActor<ABuilding>(constructionPlan->BuildingClass)->Init(constructionPlan);
    Condition = NewObject<UInBuilderMode>(this);
    Preview->AddCondition(Condition);

    TInlineComponentArray<UStaticMeshComponent*> meshes;
    Preview->GetComponents<UStaticMeshComponent>(meshes, true);
    for (const auto mesh : meshes)
        mesh->SetCollisionProfileName(CollisionProfiles::OverlapAllDynamic, true);

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
    inputComponent->BindAction("Select", IE_Pressed, this, &UBuildingBuilderMode::ConfirmPosition);
    inputComponent->BindAction("RotateBlueprint90", IE_Pressed, this, &UBuildingBuilderMode::Rotate90);
    return this;
}

bool UBuildingBuilderMode::Tick(const ACameraPawn& camera) {
    switch (Phase) {
    case Positioning:
        TickPosition(camera);
        break;
    case Waiting:
        TickWait();
        break;
    case Done:
        return true;
    default: checkNoEntry();
        return true;
    }

    CheckOverlap();
    ConstructionUI->ConstructionMaterials->UpdateHave(The::ConstructionManager(this));

    if (const auto extension = Extensions->BuildingExtension)
        extension->Update();
    for (const auto& extension : Extensions->ComponentExtensions)
        extension.Value->Update();
    return false;
}


void UBuildingBuilderMode::TickPosition(const ACameraPawn& camera) const {
    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor) {
        Preview->SetActorHiddenInGame(true);
        return;
    }
    
    FHitResult landscapeUnderCursor;
    if (!playerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(COLLISION_LANDSCAPE), false, landscapeUnderCursor)) {
        Preview->SetActorHiddenInGame(true);        
        return;
    }
    
    Preview->SetActorLocation(landscapeUnderCursor.ImpactPoint + FVector(0, 0, 1));
    Preview->SetActorHiddenInGame(false);
}

void UBuildingBuilderMode::TickWait() const {
    if (ArrowForward->IsComponentTickEnabled()) {
        ConfirmSymbol->SetVisibility(false);
        CancelSymbol->SetVisibility(false);
        ArrowSideways->SetVisibility(false);
        ArrowRotate->SetVisibility(false);
    } else if (ArrowSideways->IsComponentTickEnabled()) {
        ConfirmSymbol->SetVisibility(false);
        CancelSymbol->SetVisibility(false);
        ArrowForward->SetVisibility(false);
        ArrowRotate->SetVisibility(false);
    } else if (ArrowRotate->IsComponentTickEnabled()) {
        ConfirmSymbol->SetVisibility(false);
        CancelSymbol->SetVisibility(false);
        ArrowForward->SetVisibility(false);
        ArrowSideways->SetVisibility(false);
    } else {
        ConfirmSymbol->SetVisibility(!HasOverlap);
        CancelSymbol->SetVisibility(true);
        ArrowForward->SetVisibility(true);
        ArrowSideways->SetVisibility(true);
        ArrowRotate->SetVisibility(true);
    }
}

void UBuildingBuilderMode::CheckOverlap() {
    if (Preview->IsHidden())
        return;

    Preview->UpdateOverlaps(false);
    
    TSet<AActor*> overlaps;
    Preview->GetOverlappingActors(overlaps);
    
    // TODO filter out overlaps that are okay, like ships & foliage

    if (!HasOverlap && overlaps.Num() > 0) {
        // change to overlapping
        HasOverlap = true;
        Preview->AddCondition(HighlightedInvalid);
        if (Phase == Waiting)
            ConfirmSymbol->SetVisibility(false);
    } else if (HasOverlap && overlaps.Num() == 0) {
        // change to not overlapping
        HasOverlap = false;
        Preview->RemoveCondition(HighlightedInvalid);
        if (Phase == Waiting)
            ConfirmSymbol->SetVisibility(true);
    }
}

UClass* UBuildingBuilderMode::IDK() {
    return ConstructionPlan->BuildingClass;
}

void UBuildingBuilderMode::ConfirmPosition() {
    const auto playerController = The::PlayerController(this);;
    const TObjectPtr<UInputComponent> inputComponent = playerController->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);

    UImageUI* confirmImageUI = CreateWidget<UImageUI>(playerController, playerController->BlueprintHolder->ImageUIClass);
    confirmImageUI->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Confirm")));
    confirmImageUI->Button->OnClicked.AddDynamic(this, &UBuildingBuilderMode::OnClickConfirm);
    // TODO has a copy in PowerOverlay
    ConfirmSymbol = NewObject<UWidgetComponent>(Preview);
    ConfirmSymbol->RegisterComponent();
    ConfirmSymbol->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    ConfirmSymbol->SetWidgetSpace(EWidgetSpace::Screen);
    ConfirmSymbol->SetWidget(confirmImageUI);
    ConfirmSymbol->SetDrawSize(FVector2D(80, 80)); // TODO scale every tick based on distance to camera
    ConfirmSymbol->SetPivot(FVector2D(-0.5, -0.5));
    ConfirmSymbol->SetGenerateOverlapEvents(false);
    ConfirmSymbol->RecreatePhysicsState(); // without this, some of the arrows below are not clickable, no idea why
    Preview->AddInstanceComponent(ConfirmSymbol);

    UImageUI* cancelImageUI = CreateWidget<UImageUI>(playerController, playerController->BlueprintHolder->ImageUIClass);
    cancelImageUI->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Cancel")));
    cancelImageUI->Button->OnClicked.AddDynamic(this, &UBuildingBuilderMode::OnClickCancel);
    CancelSymbol = NewObject<UWidgetComponent>(Preview);
    CancelSymbol->RegisterComponent();
    CancelSymbol->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    CancelSymbol->SetWidgetSpace(EWidgetSpace::Screen);
    CancelSymbol->SetWidget(cancelImageUI);
    CancelSymbol->SetDrawSize(FVector2D(80, 80)); // TODO scale every tick based on distance to camera
    CancelSymbol->SetPivot(FVector2D(1.5, -0.5));
    CancelSymbol->SetGenerateOverlapEvents(false);
    CancelSymbol->RecreatePhysicsState(); // without this, some of the arrows below are not clickable, no idea why
    Preview->AddInstanceComponent(CancelSymbol);

    ArrowForward = NewObject<UArrowMoverLine>(Preview);
    ArrowForward->RegisterComponent();
    ArrowForward->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    ArrowForward->SetRelativeLocation(FVector(60, 0, 50));
    ArrowForward->SetWorldScale3D(FVector(5));
    Preview->AddInstanceComponent(ArrowForward);

    ArrowSideways = NewObject<UArrowMoverLine>(Preview);
    ArrowSideways->RegisterComponent();
    ArrowSideways->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    ArrowSideways->SetRelativeRotation(FRotator(0, 90, 0));
    ArrowSideways->SetRelativeLocation(FVector(0, 60, 50));
    ArrowSideways->SetWorldScale3D(FVector(5));
    Preview->AddInstanceComponent(ArrowSideways);

    ArrowRotate = NewObject<UArrowMoverRotate>(Preview);
    ArrowRotate->RegisterComponent();
    ArrowRotate->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    ArrowRotate->SetRelativeLocation(FVector(60, 60, 50));
    ArrowRotate->SetWorldScale3D(FVector(5));
    Preview->AddInstanceComponent(ArrowRotate);

    // bind keys
    inputComponent->BindAction("PushBlueprintUp", IE_Pressed, this, &UBuildingBuilderMode::PushUp);
    inputComponent->BindAction("PushBlueprintDown", IE_Pressed, this, &UBuildingBuilderMode::PushDown);
    inputComponent->BindAction("PushBlueprintLeft", IE_Pressed, this, &UBuildingBuilderMode::PushLeft);
    inputComponent->BindAction("PushBlueprintRight", IE_Pressed, this, &UBuildingBuilderMode::PushRight);
    inputComponent->BindAction("RotateBlueprintLeft", IE_Pressed, this, &UBuildingBuilderMode::RotateLeft);
    inputComponent->BindAction("RotateBlueprintLeft", IE_Repeat, this, &UBuildingBuilderMode::RotateLeft);
    inputComponent->BindAction("RotateBlueprintRight", IE_Pressed, this, &UBuildingBuilderMode::RotateRight);
    inputComponent->BindAction("RotateBlueprintRight", IE_Repeat, this, &UBuildingBuilderMode::RotateRight);

    Phase = Waiting;
}

void UBuildingBuilderMode::RemoveBindingsWaiting() const {
    const TObjectPtr<UInputComponent> inputComponent = The::PlayerController(this)->InputComponent;
    inputComponent->RemoveActionBinding("PushBlueprintUp", IE_Pressed);
    inputComponent->RemoveActionBinding("PushBlueprintDown", IE_Pressed);
    inputComponent->RemoveActionBinding("PushBlueprintLeft", IE_Pressed);
    inputComponent->RemoveActionBinding("PushBlueprintRight", IE_Pressed);
    inputComponent->RemoveActionBinding("RotateBlueprintLeft", IE_Pressed);
    inputComponent->RemoveActionBinding("RotateBlueprintLeft", IE_Repeat);
    inputComponent->RemoveActionBinding("RotateBlueprintRight", IE_Pressed);
    inputComponent->RemoveActionBinding("RotateBlueprintRight", IE_Repeat);
}

void UBuildingBuilderMode::PushUp() {
    Preview->AddActorLocalOffset(FVector(5, 0, 0));
}

void UBuildingBuilderMode::PushDown() {
    Preview->AddActorLocalOffset(FVector(-5, 0, 0));
}

void UBuildingBuilderMode::PushLeft() {
    Preview->AddActorLocalOffset(FVector(0, -5, 0));
}

void UBuildingBuilderMode::PushRight() {
    Preview->AddActorLocalOffset(FVector(0, 5, 0));
}

void UBuildingBuilderMode::RotateLeft() {
    Preview->AddActorLocalRotation(FRotator(0, -5, 0));
}

void UBuildingBuilderMode::RotateRight() {
    Preview->AddActorLocalRotation(FRotator(0, 5, 0));
}

void UBuildingBuilderMode::Rotate90() {
    Preview->AddActorLocalRotation(FRotator(0, 90, 0));
}

void UBuildingBuilderMode::OnClickConfirm() {
    if (HasOverlap)
        return;

    Stop(false);
    Preview->RemoveCondition(Condition);

    // create and add construction site
    NewObject<UConstructionSite>(Preview)->Init(Preview, ConstructionPlan, Extensions)->QueueTasks();
}

void UBuildingBuilderMode::OnClickCancel() {
    Stop(true);
}

void UBuildingBuilderMode::Stop(bool cancelled) {
    if (Phase == Done)
        return; // everything is done

    if (const auto extension = Extensions->BuildingExtension)
        extension->End(cancelled);
    for (const auto& extension : Extensions->ComponentExtensions)
        extension.Value->End(cancelled);

    if (!cancelled) {
        // remove components
        ConfirmSymbol->DestroyComponent();
        ConfirmSymbol = nullptr;
        CancelSymbol->DestroyComponent();
        CancelSymbol = nullptr;
        ArrowForward->DestroyComponent();
        ArrowForward = nullptr;
        ArrowSideways->DestroyComponent();
        ArrowSideways = nullptr;
        ArrowRotate->DestroyComponent();
        ArrowRotate = nullptr;

        // set collision
        TInlineComponentArray<UStaticMeshComponent*> meshes;
        Preview->GetComponents<UStaticMeshComponent>(meshes, true);
        for (const auto mesh : meshes)
            mesh->SetCollisionProfileName(CollisionProfiles::BlockAllDynamic, true);
    } else {
        // remove Preview and all components
        Preview->Destroy();
    }

    const auto playerController = The::PlayerController(this);
    playerController->Deselect();
    
    // undo all input bindings
    const TObjectPtr<UInputComponent> inputComponent = playerController->InputComponent;
    inputComponent->RemoveActionBinding("RotateBlueprint90", EInputEvent::IE_Pressed);

    if (Phase == Positioning) {
        inputComponent->RemoveActionBinding("Select", IE_Pressed);
    } else if (Phase == Waiting) {
        RemoveBindingsWaiting();
    }
    
    Phase = Done;
}
