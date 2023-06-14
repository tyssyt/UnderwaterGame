#include "BuilderMode.h"

#include "ArrowMover.h"
#include "Components/WidgetComponent.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Conveyor.h"
#include "XD/Buildings/Habitat.h"

#include "XD/GameInstanceX.h"
#include "XD/Buildings/Splitter.h"

// BuildingBuilderMode

UBuildingBuilderMode::UBuildingBuilderMode() {
    static const FName name = FName(TEXT("Base Material Dynamic"));
    static ConstructorHelpers::FObjectFinderOptional<UMaterial> GhostMaterial(TEXT("/Game/GhostMaterial"));
    HighlightMaterial = UMaterialInstanceDynamic::Create(GhostMaterial.Get(), this, name);
    static const FName ParameterNameColor = FName(TEXT("BaseColor"));
    HighlightMaterial->SetVectorParameterValue(ParameterNameColor, FLinearColor::Red);
}

UBuildingBuilderMode* UBuildingBuilderMode::Init(UConstructionPlan* constructionPlan, UWorld* world) {
    ConstructionPlan = constructionPlan;
    Preview = world->SpawnActor<ABuilding>(constructionPlan->BuildingClass);
    Preview->SetActorTickEnabled(false);

    TInlineComponentArray<UStaticMeshComponent*> meshes;
    Preview->GetComponents<UStaticMeshComponent>(meshes, true);
    for (auto mesh : meshes) {
        const static FName CollisionProfileName(TEXT("OverlapAllDynamic"));
        mesh->SetCollisionProfileName(CollisionProfileName, true);
    }

    // bind keys
    const auto inputComponent = world->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UBuildingBuilderMode::ConfirmPosition);
    inputComponent->BindAction("RotateBlueprint90", IE_Pressed, this, &UBuildingBuilderMode::Rotate90);
    return this;
}

bool UBuildingBuilderMode::Tick(const ACameraPawn& camera) {
    switch (Phase) {
    case Positioning:
        Position(camera);
        CheckOverlap();
        return false;
    case Waiting:
        Wait();
        CheckOverlap();
        return false;
    case Done:
        return true;
    default: checkNoEntry();
        return true;
    }
}


void UBuildingBuilderMode::Position(const ACameraPawn& camera) const {
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

void UBuildingBuilderMode::Wait() const {
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
        Preview->SetAllMaterials(HighlightMaterial);
        if (Phase == Waiting)
            ConfirmSymbol->SetVisibility(false);
    } else if (HasOverlap && overlaps.Num() == 0) {
        // change to not overlapping
        HasOverlap = false;
        Preview->SetAllMaterials(nullptr);
        if (Phase == Waiting)
            ConfirmSymbol->SetVisibility(true);
    }
}

UClass* UBuildingBuilderMode::IDK() {
    return ConstructionPlan->BuildingClass;
}

void UBuildingBuilderMode::ConfirmPosition() {
    APlayerControllerX* playerController = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>();
    const TObjectPtr<UInputComponent> inputComponent = playerController->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);
    
    UImageUI* confirmImageUI = CreateWidget<UImageUI>(playerController, playerController->BlueprintHolder->ImageUIClass);
    confirmImageUI->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Confirm")));
    confirmImageUI->Button->OnClicked.AddDynamic(this, &UBuildingBuilderMode::OnClickConfirm);
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
    TObjectPtr<UInputComponent> inputComponent = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
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

    // undo all input bindings
    TObjectPtr<UInputComponent> inputComponent = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->RemoveActionBinding("RotateBlueprint90", IE_Pressed);
    RemoveBindingsWaiting();

    TInlineComponentArray<UStaticMeshComponent*> meshes;
    Preview->GetComponents<UStaticMeshComponent>(meshes, true);
    for (auto mesh : meshes) {
        const static FName CollisionProfileName(TEXT("BlockAllDynamic"));
        mesh->SetCollisionProfileName(CollisionProfileName, true);
    }

    // create and add construction site
    ConstructionSite* constructionSite = new ConstructionSite(Preview, ConstructionPlan);
    Preview->GetWorld()->GetGameInstance<UGameInstanceX>()->TheConstructionManager->AddConstruction(constructionSite);
    Phase = Done;
}

void UBuildingBuilderMode::OnClickCancel() {
    // remove Preview and all components
    Preview->Destroy();

    // undo all input bindings
    TObjectPtr<UInputComponent> inputComponent = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->RemoveActionBinding("RotateBlueprint90", IE_Pressed);
    RemoveBindingsWaiting();

    Phase = Done;
}

void UBuildingBuilderMode::Stop() {
    if (Phase == Done) {
        return; // everything is done
    }

    Preview->Destroy();

    TObjectPtr<UInputComponent> inputComponent = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->RemoveActionBinding("RotateBlueprint90", EInputEvent::IE_Pressed);

    if (Phase == Positioning) {
        inputComponent->RemoveActionBinding("Select", IE_Pressed);
    } else if (Phase == Waiting) {
        RemoveBindingsWaiting();
    }
}

// ConveyorBuilderMode

UConveyorBuilderMode::UConveyorBuilderMode() {
    static const FName name = FName(TEXT("Base Material Dynamic"));
    static const FName ParameterNameColor = FName(TEXT("BaseColor"));
    static ConstructorHelpers::FObjectFinderOptional<UMaterial> GhostMaterial(TEXT("/Game/GhostMaterial"));
    RedMaterial = UMaterialInstanceDynamic::Create(GhostMaterial.Get(), this, name);
    RedMaterial->SetVectorParameterValue(ParameterNameColor, FLinearColor::Red);
    GreenMaterial = UMaterialInstanceDynamic::Create(GhostMaterial.Get(), this, name);
    GreenMaterial->SetVectorParameterValue(ParameterNameColor, FLinearColor::Green);
}

UConveyorBuilderMode* UConveyorBuilderMode::Init() {
    // bind keys
    const auto inputComponent = GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UConveyorBuilderMode::Confirm);
    return this;
}

void UConveyorBuilderMode::CreateNextLinkAndNode() {
    const static FName CollisionProfileName(TEXT("OverlapAllDynamic"));
    USceneComponent* sceneComponent = Preview->GetRootComponent();
    NextLink = NewObject<UConveyorLink>(Preview);
    NextLink->RegisterComponent();
    NextLink->AttachToComponent(sceneComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    NextLink->SetCollisionProfileName(CollisionProfileName, true);
    Preview->AddInstanceComponent(NextLink);

    NextNode = NewObject<UConveyorNode>(Preview);
    NextNode->RegisterComponent();
    NextNode->AttachToComponent(sceneComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    NextNode->SetCollisionProfileName(CollisionProfileName, true);
    Preview->AddInstanceComponent(NextNode);
}

bool UConveyorBuilderMode::Tick(const ACameraPawn& camera) {
    if (Done)
        return true;
    
    if (Source.Type == SourceTarget::EType::NotSet) {
        TickSelectSource(camera);
    } else if (Target.Type == SourceTarget::EType::NotSet) {
        TickSelectNextPoint(camera);
        CheckOverlap();
    } else {
        TickInsertNode(camera);
    }
    return false;
}

void UConveyorBuilderMode::TickSelectSource(const ACameraPawn& camera) {
    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController)
        return;
    
    HighlightUnderCursor(playerController, true);
}

void UConveyorBuilderMode::TickSelectNextPoint(const ACameraPawn& camera) {
    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController)
        return;
    
    if (HighlightUnderCursor(playerController, false)) {
        if (CurrentHighlightValid) {
            DrawNextLink(CurrentHighlight.Building->GetActorLocation(), false);
        } else {
            NextNode->SetVisibility(false);
            NextLink->SetVisibility(false);                
        }
    } else {
        FHitResult landscapeUnderCursor;
        if (playerController->GetHitResultUnderCursorByChannel(
            UEngineTypes::ConvertToTraceType(COLLISION_LANDSCAPE),
            false,
            landscapeUnderCursor
        )) {
            // keep z-Offset in sync with UArrowMoverUp
            DrawNextLink(landscapeUnderCursor.ImpactPoint + FVector(0, 0, 15), true);
        } else {
            NextNode->SetVisibility(false);
            NextLink->SetVisibility(false);                
        }
    }
}

void UConveyorBuilderMode::TickInsertNode(const ACameraPawn& camera) {   
    // Do not show new Node Preview if any Mover is active
    TInlineComponentArray<UArrowMover*> movers;
    Preview->GetComponents<UArrowMover>(movers, true);
    for (UArrowMover* mover : movers) {
        if (mover->IsComponentTickEnabled()) {
            NextNode->SetVisibility(false);
            InsertNodeHoverLink = nullptr;
            // the invisible node can still cause collision, so we move it away
            NextNode->SetWorldLocation(camera.GetActorLocation() + FVector(0, 0, 5000));
            return;            
        }
    }

    // Show new Node Preview if the Mouse is over a Link
    APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    FHitResult hitResult;
    if (playerController->GetHitResultUnderCursor(ECC_Visibility, true, hitResult)
            && hitResult.GetActor() == Preview
            && hitResult.Component->IsA<UConveyorLink>()) {
        UConveyorLink* link = Cast<UConveyorLink>(hitResult.Component);
        NextNode->SetWorldLocation(ProjectOntoLink(hitResult.ImpactPoint, link));
        NextNode->SetVisibility(true);
        InsertNodeHoverLink = link;
    } else {
        NextNode->SetVisibility(false);
        InsertNodeHoverLink = nullptr;
        // the invisible node can still cause collision, so we move it away
        NextNode->SetWorldLocation(camera.GetActorLocation() + FVector(0, 0, 5000));
    }
}

FVector UConveyorBuilderMode::ProjectOntoLink(FVector loc, UConveyorLink* link) {
    const FVector meshLocation = link->GetComponentLocation();
    const FVector projection = (loc - meshLocation).ProjectOnToNormal(link->GetComponentRotation().Vector());
    return meshLocation + projection;
}

ABuilding* UConveyorBuilderMode::SpawnSplitter(bool isSource, const UResource* resource) const {
    ABuilding* building;
    if (CurrentHighlight.Type == SourceTarget::EType::ConveyorLink) {
        building = CurrentHighlight.Building;
    } else {
        if (isSource)
            building = GetWorld()->SpawnActor<ASplitter>();
        else
            building = GetWorld()->SpawnActor<AMerger>();
        building->SetActorTickEnabled(false);
        const static FName CollisionProfileName(TEXT("OverlapAllDynamic"));
        building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfileName, true);
    }
    
    UInventoryComponent* inventory = building->GetComponentByClass<UInventoryComponent>();    
    for (auto& input : inventory->GetInputs())
        input.Resource = resource;
    for (auto& output : inventory->GetOutputs())
        output.Resource = resource;
    return building;
}

bool UConveyorBuilderMode::HighlightUnderCursor(const APlayerControllerX* playerController, bool isSource) {
    FHitResult hitResult;
    if (!playerController->GetHitResultUnderCursor(ECC_Visibility, true, hitResult)) {
        RemoveCurrentHighlight();
        return false;
    }

    // Check if Cursor is over a Conveyor, if so we Start with a Splitter
    if (AConveyor* conveyor = Cast<AConveyor>(hitResult.GetActor())) {

        if (UConveyorNode* node = Cast<UConveyorNode>(hitResult.GetComponent())) {
            
            if (CurrentHighlight.Type == SourceTarget::EType::ConveyorNode && CurrentHighlight.Conveyor == conveyor && CurrentHighlight.ConveyorComponent == node)
                return true; // already highlighted

            ABuilding* building = SpawnSplitter(isSource, conveyor->SourceInv->Resource);
            building->SetActorLocation(node->GetComponentLocation());

            if (isSource) {
                SetHighlight(building, CheckValidSource(building));
            } else {                
                if (conveyor == Source.Conveyor) {
                    // TODO The current splitting on construction cannot handle 2 splits in the same conveyor
                    SetHighlight(building, false);
                } else {
                    Resource = CheckValidTarget(building);
                    SetHighlight(building, Resource != nullptr);
                }
            }
            
            CurrentHighlight.Type = SourceTarget::EType::ConveyorNode;
            CurrentHighlight.Conveyor = conveyor;
            CurrentHighlight.ConveyorComponent = node;

            // hide the original conveyor node, because the splitter is right on top of it
            node->SetVisibility(false);
            return true;
        }
        
        if (UConveyorLink* link = Cast<UConveyorLink>(hitResult.GetComponent())) {
            
            ABuilding* building = SpawnSplitter(isSource, conveyor->SourceInv->Resource);;
            building->SetActorLocation(ProjectOntoLink(hitResult.ImpactPoint, link));

            if (isSource) {
                if (CheckOverlap(building->GetComponentByClass<UStaticMeshComponent>(), {link}, {}))
                    SetHighlight(building, false);
                else
                    SetHighlight(building, CheckValidSource(building));
            } else {
                if (conveyor == Source.Conveyor // TODO The current splitting on construction cannot handle 2 splits in the same conveyor
                    || CheckOverlap(building->GetComponentByClass<UStaticMeshComponent>(), {link, NextLink, NextNode}, {})) {
                    SetHighlight(building, false);
                } else {
                    Resource = CheckValidTarget(building);
                    SetHighlight(building, Resource != nullptr);
                }
            }
            
            CurrentHighlight.Type = SourceTarget::EType::ConveyorLink;
            CurrentHighlight.Conveyor = conveyor;
            CurrentHighlight.ConveyorComponent = link;

            return true;
        }
        
        checkNoEntry();
        return false;
    }
    
    ABuilding* underCursor = Cast<ABuilding>(hitResult.GetActor());
    if (!underCursor) {
        RemoveCurrentHighlight();
        return false;
    }
    
    if (underCursor == CurrentHighlight.Building)
        return true;

    if (isSource) {
        SetHighlight(underCursor, CheckValidSource(underCursor));
    } else {
        Resource = CheckValidTarget(underCursor);
        SetHighlight(underCursor, Resource != nullptr);
    }
    CurrentHighlight.Type = SourceTarget::EType::Building;
    return true;
}

void UConveyorBuilderMode::DrawNextLink(FVector target, bool drawNode) const {
    // draw Link
    NextLink->Connect(NextLinkStartPos, target);
    NextLink->SetVisibility(true);

    // draw Node
    if (drawNode) {
        NextNode->SetWorldLocation(target);
        NextNode->SetVisibility(true);
    } else {
        NextNode->SetVisibility(false);
        // the invisible node can still cause collision, so we move it away
        NextNode->SetWorldLocation(target + FVector(0, 0, 5000));
    }
}

void UConveyorBuilderMode::CheckOverlap() {
    HasOverlap = CheckOverlapLinks() | CheckOverlapNodes(); // we need both sides to always run, so just a single |
    
    if (ConfirmSymbol)
        ConfirmSymbol->SetVisibility(!HasOverlap);
}
bool UConveyorBuilderMode::CheckOverlapLinks() {
    const bool targetSet = Target.Type != SourceTarget::EType::NotSet;
    if (targetSet && Links.Num() == 1) {
        // special case for 1 segment conveyor
        return CheckOverlap(Links[0], {}, {&Source, &Target});
    }
    
    bool hasOverlapLinks = false;
    if (Links.Num() > 0) {
        // The first Link is allowed to overlap the Source and the first Node
        hasOverlapLinks |= CheckOverlap(Links[0], {Nodes[0]}, {&Source});
    }
    const int numNormalLink = targetSet ? Links.Num() - 1 : Links.Num();
    for (int i=1; i < numNormalLink; ++i) {
        // Normal Links may overlap the Nodes before and after them
        hasOverlapLinks |= CheckOverlap(Links[i], {Nodes[i-1], Nodes[i]}, {});
    }
    if (targetSet && Links.Num() > 1) {
        // if the Target is set, the last Link may overlap the Target and the last Node
        hasOverlapLinks |= CheckOverlap(Links.Last(), {Nodes.Last()}, {&Target}); 
    }

    if (!targetSet && NextLink->IsVisible()) {
        std::vector<UStaticMeshComponent*> allowedMeshes;
        std::vector<SourceTarget*> allowed;

        // Next Link may overlap the last Node, if there are no Nodes it may overlap the Source
        if (Nodes.Num() > 0)
            allowedMeshes.push_back(Nodes.Last());
        else
            allowed.push_back(&Source);

        if (CurrentHighlight.Type != SourceTarget::EType::NotSet)
            allowed.push_back(&CurrentHighlight);

        if (NextNode->IsVisible())
            allowedMeshes.push_back(NextNode);
        
        hasOverlapLinks |= CheckOverlap(NextLink, allowedMeshes, allowed);         
    }

    return hasOverlapLinks;
}
bool UConveyorBuilderMode::CheckOverlapNodes() {
    const bool targetSet = Target.Type != SourceTarget::EType::NotSet;
    bool hasOverlapNodes = false;
    if (Nodes.Num() > 0) {
        for (int i=0; i < Nodes.Num()-1; ++i) {
            // Normal Nodes may overlap the link before and after them
            hasOverlapNodes |= CheckOverlap(Nodes[i], {Links[i], Links[i+1]}, {});
        }
        if (targetSet) {
            // if the Target is set, the last nodes behaves like a normal Node
            hasOverlapNodes |= CheckOverlap(Nodes.Last(), {Links[Links.Num()-2], Links.Last()}, {});
        } else {
            // otherwise, the last Node may overlap NextLink
            hasOverlapNodes |= CheckOverlap(Nodes.Last(), {Links.Last(), NextLink}, {});
        }
    }

    if (!targetSet && NextNode->IsVisible()) {
        // NextNode may overlap NextLink
        hasOverlapNodes |= CheckOverlap(NextNode, {NextLink}, {});
    }
    
    return hasOverlapNodes;
}

bool UConveyorBuilderMode::CheckOverlap(UStaticMeshComponent* mesh, std::vector<UStaticMeshComponent*> allowedMeshes, std::vector<SourceTarget*> allowed) const {
    mesh->UpdateOverlaps(nullptr, false);
    TSet<UPrimitiveComponent*> overlaps;
    mesh->GetOverlappingComponents(overlaps);

    std::vector<AActor*> allowedActors;
    for (const auto a : allowed) {
        allowedActors.push_back(a->Building);
        if (a->Type == SourceTarget::EType::ConveyorNode)
            allowedMeshes.push_back(a->ConveyorComponent);
    }
        
    bool hasOverlap = false;        
    for (UPrimitiveComponent* overlap : overlaps) {
        if (std::find(allowedMeshes.begin(), allowedMeshes.end(), overlap) != allowedMeshes.end())
            continue;
        if (std::find(allowedActors.begin(), allowedActors.end(), overlap->GetAttachmentRootActor()) != allowedActors.end())
            continue;
        hasOverlap = true;

        UE_LOG(LogTemp, Warning, TEXT("%s overlaps with %s on %s"), *mesh->GetName(), *overlap->GetAttachmentRootActor()->GetName(), *overlap->GetName());
        
        break;
    }    
    
    if (hasOverlap) {
        for (int i=0; i < mesh->GetMaterials().Num(); ++i)
            mesh->SetMaterial(i, RedMaterial);
    } else {   
        for (int i=0; i < mesh->GetMaterials().Num(); ++i)
            mesh->SetMaterial(i, nullptr);
    }

    return hasOverlap;
}

void UConveyorBuilderMode::AddArrowsToNode(UConveyorNode* node, UTexture2D* cancelTexture) {
    APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();
    UArrowMoverLine* arrowForward = NewObject<UArrowMoverLine>(Preview);
    arrowForward->RegisterComponent();
    arrowForward->AttachToComponent(node, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    arrowForward->AddWorldOffset(FVector(45, 0, 35));
    arrowForward->SetWorldScale3D(FVector(5));
    Preview->AddInstanceComponent(arrowForward);
    arrowForward->OnArrowMoved.BindUObject(this, &UConveyorBuilderMode::UpdateLinks, node);

    UArrowMoverLine* arrowSideways = NewObject<UArrowMoverLine>(Preview);
    arrowSideways->RegisterComponent();
    arrowSideways->AttachToComponent(node, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    arrowSideways->AddWorldOffset(FVector(0, 45, 35));
    arrowSideways->SetWorldScale3D(FVector(5));
    arrowSideways->SetRelativeRotation(FRotator(0, 90, 0));
    Preview->AddInstanceComponent(arrowSideways);                
    arrowSideways->OnArrowMoved.BindUObject(this, &UConveyorBuilderMode::UpdateLinks, node);
            
    UArrowMoverUp* arrowUp = NewObject<UArrowMoverUp>(Preview);
    arrowUp->RegisterComponent();
    arrowUp->AttachToComponent(node, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    arrowUp->AddWorldOffset(FVector(0, 0, 55));
    arrowUp->SetWorldScale3D(FVector(5));
    Preview->AddInstanceComponent(arrowUp);
    arrowUp->OnArrowMoved.BindUObject(this, &UConveyorBuilderMode::UpdateLinks, node);

    UImageUI* removeImageUI = CreateWidget<UImageUI>(playerController, playerController->BlueprintHolder->ImageUIClass);
    removeImageUI->Image->SetBrushFromTexture(cancelTexture);
    UWidgetComponent* removeSymbol = NewObject<UWidgetComponent>(Preview);
    removeSymbol->RegisterComponent();
    removeSymbol->AttachToComponent(node, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    removeSymbol->SetWidgetSpace(EWidgetSpace::Screen);
    removeSymbol->SetWidget(removeImageUI);
    removeSymbol->SetDrawSize(FVector2D(80, 80)); // TODO scale every tick based on distance to camera
    removeSymbol->SetPivot(FVector2D(0., -0.5));
    removeSymbol->SetGenerateOverlapEvents(false);
    removeSymbol->RecreatePhysicsState(); // without this, some of the arrows are not clickable, no idea why
    Preview->AddInstanceComponent(removeSymbol);

    UButtonWorkaround* buttonWorkaround = NewObject<UButtonWorkaround>(this);
    buttonWorkaround->BuilderMode = this;
    buttonWorkaround->Parent = node;
    removeImageUI->Button->OnClicked.AddDynamic(buttonWorkaround, &UButtonWorkaround::OnClick);
    ButtonWorkarounds.Push(buttonWorkaround);
}

void UConveyorBuilderMode::Confirm() {
    if (Source.Type == SourceTarget::EType::NotSet) {
        if (CurrentHighlight.Type == SourceTarget::EType::NotSet || !CurrentHighlightValid)
            return;

        Source = CurrentHighlight;
        CurrentHighlight.Type = SourceTarget::EType::NotSet;
        CurrentHighlight.Building = nullptr;
        CurrentHighlight.Conveyor = nullptr;
        CurrentHighlight.ConveyorComponent = nullptr;
        
        // TODO if the building/conveyor is under construction, this should restore the blue ghost material of construction
        Source.Building->SetAllMaterials(nullptr); 
        NextLinkStartPos = Source.Building->GetActorLocation();        

        Preview = GetWorld()->SpawnActor<AActor>();
        Preview->SetActorTickEnabled(false);
        Preview->SetRootComponent(NewObject<USceneComponent>(Preview));
        CreateNextLinkAndNode();
        return;
    }

    if (Target.Type == SourceTarget::EType::NotSet) {
        if (CurrentHighlight.Type == SourceTarget::EType::NotSet) {
            if (NextNode->IsVisible() && NextLink->IsVisible()) {
                // create new Node
                Nodes.Push(NextNode);
                Links.Push(NextLink);
                NextLinkStartPos = NextNode->GetComponentLocation();
                CreateNextLinkAndNode();
            }
            return;
        }

        if (!CurrentHighlightValid)
            return;

        Target = CurrentHighlight;
        CurrentHighlight.Type = SourceTarget::EType::NotSet;
        CurrentHighlight.Building = nullptr;
        CurrentHighlight.Conveyor = nullptr;
        CurrentHighlight.ConveyorComponent = nullptr;
        
        // TODO if the building/conveyor is under construction, this should restore the blue ghost material of construction
        Target.Building->SetAllMaterials(nullptr); 
        
        Links.Push(NextLink);
        NextLink = nullptr;
        
        for (int i=0; i < NextNode->GetMaterials().Num(); ++i)
            NextNode->SetMaterial(i, nullptr);
        const static FName CollisionProfileName(TEXT("UI"));
        for (UStaticMeshComponent* link : Links)          
            link->SetCollisionProfileName(CollisionProfileName, true);

        UTexture2D* cancelTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Cancel"));
        // create arrows over all Nodes
        for (auto node : Nodes)
            AddArrowsToNode(node, cancelTexture);  

        // create confirm / cancel over target
        APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();
        
        UImageUI* confirmImageUI = CreateWidget<UImageUI>(playerController, playerController->BlueprintHolder->ImageUIClass);
        confirmImageUI->Image->SetBrushFromTexture(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Confirm")));
        confirmImageUI->Button->OnClicked.AddDynamic(this, &UConveyorBuilderMode::OnClickConfirm);
        ConfirmSymbol = NewObject<UWidgetComponent>(Preview);
        ConfirmSymbol->RegisterComponent();
        ConfirmSymbol->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
        ConfirmSymbol->SetWidgetSpace(EWidgetSpace::Screen);
        ConfirmSymbol->SetWidget(confirmImageUI);
        ConfirmSymbol->SetWorldLocation(Target.Building->GetActorLocation());
        ConfirmSymbol->SetDrawSize(FVector2D(80, 80)); // TODO scale every tick based on distance to camera
        ConfirmSymbol->SetPivot(FVector2D(-0.5, -0.5));
        ConfirmSymbol->SetGenerateOverlapEvents(false);
        ConfirmSymbol->RecreatePhysicsState(); // without this, some of the arrows below are not clickable, no idea why
        ConfirmSymbol->SetVisibility(!HasOverlap);
        Preview->AddInstanceComponent(ConfirmSymbol);

        UImageUI* cancelImageUI = CreateWidget<UImageUI>(playerController, playerController->BlueprintHolder->ImageUIClass);
        cancelImageUI->Image->SetBrushFromTexture(cancelTexture);
        cancelImageUI->Button->OnClicked.AddDynamic(this, &UConveyorBuilderMode::OnClickCancel);
        UWidgetComponent* cancelSymbol = NewObject<UWidgetComponent>(Preview);
        cancelSymbol->RegisterComponent();
        cancelSymbol->AttachToComponent(Preview->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
        cancelSymbol->SetWidgetSpace(EWidgetSpace::Screen);
        cancelSymbol->SetWidget(cancelImageUI);
        cancelSymbol->SetWorldLocation(Target.Building->GetActorLocation());
        cancelSymbol->SetDrawSize(FVector2D(80, 80)); // TODO scale every tick based on distance to camera
        cancelSymbol->SetPivot(FVector2D(1.5, -0.5));
        cancelSymbol->SetGenerateOverlapEvents(false);
        cancelSymbol->RecreatePhysicsState(); // without this, some of the arrows below are not clickable, no idea why
        Preview->AddInstanceComponent(cancelSymbol);

        return;
    }

    if (InsertNodeHoverLink) {
        auto idx = Links.Find(InsertNodeHoverLink);
        check(idx != INDEX_NONE);

        UConveyorNode* insertNode = NextNode;
        AddArrowsToNode(insertNode, LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Cancel")));
        if (idx < Nodes.Num())
            Nodes.Insert(insertNode, idx);
        else
            Nodes.Push(insertNode);

        CreateNextLinkAndNode();
        
        const static FName CollisionProfileName(TEXT("UI"));
        NextLink->SetCollisionProfileName(CollisionProfileName, true);
        Links.Insert(NextLink, idx);
        NextLink = nullptr;

        UpdateLinks(insertNode);
    }
}

void UConveyorBuilderMode::SetHighlight(ABuilding* building, bool valid) {
    if (CurrentHighlight.Building != building) {
        RemoveCurrentHighlight();
        CurrentHighlight.Building = building;
    }
    CurrentHighlightValid = valid;
    building->SetAllMaterials(valid ? GreenMaterial : RedMaterial);
}

void UConveyorBuilderMode::RemoveCurrentHighlight() {
    // TODO if the building/conveyor is under construction, this should restore the blue ghost material of construction
    // or instead, just disallow selecting buildings/conveyors under construction?
    switch (CurrentHighlight.Type) {
    case SourceTarget::EType::NotSet:
        return;
    case SourceTarget::EType::Building:
        CurrentHighlight.Building->SetAllMaterials(nullptr);
        CurrentHighlight.Building = nullptr;
        break;
    case SourceTarget::EType::ConveyorNode:
        CurrentHighlight.ConveyorComponent->SetVisibility(true);
        // fallthrough
    case SourceTarget::EType::ConveyorLink:
        CurrentHighlight.Building->Destroy();
        CurrentHighlight.Building = nullptr;
        CurrentHighlight.Conveyor = nullptr;
        CurrentHighlight.ConveyorComponent = nullptr;
        break;
    default: checkNoEntry();
    }
    CurrentHighlight.Type = SourceTarget::EType::NotSet;
}

void UConveyorBuilderMode::UpdateLinks(UConveyorNode* node) {
    const auto idx = Nodes.Find(node);
    check(idx != INDEX_NONE);

    // update link before Node
    {
        UConveyorLink* link = Links[idx];
        const FVector start = idx <= 0 ? Source.Building->GetActorLocation() : Nodes[idx-1]->GetComponentLocation();
        const FVector end = node->GetComponentLocation();
        link->Connect(start, end);
    }    

    // update link after Node
    {
        UConveyorLink* link = Links[idx+1];
        const FVector start = node->GetComponentLocation();
        const FVector end = idx >= Nodes.Num()-1 ? Target.Building->GetActorLocation() : Nodes[idx+1]->GetComponentLocation();
        link->Connect(start, end);
    }
    CheckOverlap();
}

void UConveyorBuilderMode::OnClickConfirm() {
    if (Done || HasOverlap)
        return;
    
    Done = true;
    
    RemoveCurrentHighlight();
    
    if (Preview) {
        Preview->Destroy();
        Preview = nullptr;
    }

    const UGameInstanceX* gameInstance = GetWorld()->GetGameInstance<UGameInstanceX>();
    
    const static FName CollisionProfileName(TEXT("BlockAllDynamic"));
    if (Source.Type == SourceTarget::EType::ConveyorNode || Source.Type == SourceTarget::EType::ConveyorLink) {
        Source.Building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfileName, true);
        // create a construction Site for the Splitter
        ConstructionSite* constructionSite = new ConstructionSite(Source.Building, gameInstance->TheBuildingBook->Splitter);
        gameInstance->TheConstructionManager->AddConstruction(constructionSite);

        // split the conveyor
        Source.Conveyor->SplitAt(Source.ConveyorComponent, Source.Building);
    }
    
    if (Target.Type == SourceTarget::EType::ConveyorNode || Target.Type == SourceTarget::EType::ConveyorLink) {
        Target.Building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfileName, true);
        // create a construction Site for the Merger
        ConstructionSite* constructionSite = new ConstructionSite(Target.Building, gameInstance->TheBuildingBook->Merger);
        gameInstance->TheConstructionManager->AddConstruction(constructionSite);

        // split the conveyor
        Target.Conveyor->SplitAt(Target.ConveyorComponent, Target.Building);
    }
            
    // create conveyor and add construction site
    TArray<FVector> nodes;
    nodes.Reserve(Nodes.Num());
    for (auto& node : Nodes)
        nodes.Push(node->GetComponentLocation());
    AConveyor* conveyor = AConveyor::Create(GetWorld(), Source.Building, Target.Building, nodes, Resource);
    ConstructionSite* constructionSite = new ConstructionSite(
        conveyor,
        1,
        {Material(10, gameInstance->TheResourceBook->LargeParts)}
    ); // TODO make cost & time scale with length
    gameInstance->TheConstructionManager->AddConstruction(constructionSite);
    
    // unbind
    GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent->RemoveActionBinding("Select", IE_Pressed);
}

void UConveyorBuilderMode::OnClickCancel() {
    if (Done)
        return;
    
    Done = true;
    
    if (Source.Type == SourceTarget::EType::ConveyorNode) {
        Source.Building->Destroy();
        Source.ConveyorComponent->SetVisibility(true);
    } else if (Source.Type == SourceTarget::EType::ConveyorLink) {
        Source.Building->Destroy();
    }
    
    if (Target.Type == SourceTarget::EType::ConveyorNode) {
        Target.Building->Destroy();
        Target.ConveyorComponent->SetVisibility(true);
    } else if (Target.Type == SourceTarget::EType::ConveyorLink) {
        Target.Building->Destroy();
    }
    
    RemoveCurrentHighlight();
    
    if (Preview) {
        Preview->Destroy();
        Preview = nullptr;
    }

    // unbind
    GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent->RemoveActionBinding("Select", IE_Pressed);
}

bool UConveyorBuilderMode::CheckValidSource(const ABuilding* building) {
    UInventoryComponent* inventory = building->GetComponentByClass<UInventoryComponent>();
    if (!inventory)
        return false;
    for (const auto& output : inventory->GetOutputs()) { // TODO handle buffer
        if (!output.Conveyor)
            return true;
    }
    return false;
}

const UResource* UConveyorBuilderMode::CheckValidTarget(const ABuilding* building) const {
    if (building == Source.Building)
        return nullptr;
    return AConveyor::FindCommonResource(Source.Building->GetComponentByClass<UInventoryComponent>(), building->GetComponentByClass<UInventoryComponent>());
}

UClass* UConveyorBuilderMode::IDK() {
    return AConveyor::StaticClass();
}

void UConveyorBuilderMode::Stop() {
    if (Source.Type == SourceTarget::EType::ConveyorNode) {
        Source.Building->Destroy();
        Source.ConveyorComponent->SetVisibility(true);
    } else if (Source.Type == SourceTarget::EType::ConveyorLink) {
        Source.Building->Destroy();
    }
    
    if (Target.Type == SourceTarget::EType::ConveyorNode) {
        Target.Building->Destroy();
        Target.ConveyorComponent->SetVisibility(true);
    } else if (Target.Type == SourceTarget::EType::ConveyorLink) {
        Target.Building->Destroy();
    }
    
    RemoveCurrentHighlight();

    if (Preview) {
        Preview->Destroy();
        Preview = nullptr;
    }
    
    // unbind
    GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent->RemoveActionBinding("Select", IE_Pressed);
}

void UConveyorBuilderMode::RemoveNode(UConveyorNode* node) {
    const auto idx = Nodes.Find(node);
    check(idx != INDEX_NONE);

    TArray<USceneComponent*> children;
    node->GetChildrenComponents(true, children);
    for (USceneComponent* child : children)
        child->DestroyComponent();
    node->DestroyComponent();
    Nodes.RemoveAt(idx);

    Links[idx]->DestroyComponent();
    Links.RemoveAt(idx);

    if (Nodes.Num() > 0) {
        UpdateLinks(Nodes[FMath::Min(idx, Nodes.Num()-1)]);
    } else {
        Links[0]->Connect(Source.Building->GetActorLocation(), Target.Building->GetActorLocation());
        CheckOverlap();
    }
}


// IndoorBuilderMode

UIndoorBuilderMode::UIndoorBuilderMode() {    
    static const FName name = FName(TEXT("Base Material Dynamic"));
    static ConstructorHelpers::FObjectFinderOptional<UMaterial> GhostMaterial(TEXT("/Game/GhostMaterial"));
    HighlightMaterial = UMaterialInstanceDynamic::Create(GhostMaterial.Get(), this, name);
    static const FName ParameterNameColor = FName(TEXT("BaseColor"));
    HighlightMaterial->SetVectorParameterValue(ParameterNameColor, FLinearColor::Red);
}

UIndoorBuilderMode* UIndoorBuilderMode::Init(UConstructionPlan* constructionPlan, UWorld* world) {
    ConstructionPlan = constructionPlan;
    Preview = world->SpawnActor<AIndoorBuilding>(constructionPlan->BuildingClass);
    Preview->SetActorTickEnabled(false);

    // bind keys
    const auto inputComponent = world->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UIndoorBuilderMode::ConfirmPosition);
    return this;
}

bool UIndoorBuilderMode::Tick(const ACameraPawn& camera) {
    if (!Preview)
        return true;

    Preview->SetActorLocation(camera.GetActorLocation());

    // project mouse
    FHitResult hitResult;
    APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController || !playerController->bShowMouseCursor || !playerController->GetHitResultUnderCursor(ECC_Visibility, true, hitResult)) {
        SetInvisible();
        return false;
    }

    Preview->SetActorHiddenInGame(false);
    Preview->SetActorLocation(hitResult.ImpactPoint);

    // check if mouse is over habitat
    AHabitat* habitat = Cast<AHabitat>(hitResult.GetActor());
    if (!habitat) {
        SetNotBuildable();
        return false;
    }

    // get build grid coordinates
    auto gridPos = habitat->findCoordinates(hitResult.ImpactPoint);
    if (!gridPos.first) {
        SetNotBuildable();
        return false;
    }
    int x = gridPos.second.first;
    int y = gridPos.second.second;
    Preview->setCoordinates(x, y, habitat);

    if (!habitat->canPlaceBuilding(Preview)) {
        SetNotBuildable();
        return false;
    }

    SetBuildable();
    return false;
}

void UIndoorBuilderMode::Stop() {
    if (!Preview)
        return;

    Preview->Destroy();
    Preview = nullptr;
    TObjectPtr<UInputComponent> inputComponent = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);
}

void UIndoorBuilderMode::ConfirmPosition() {
    if (!Buildable) {
        return;
    }

    TObjectPtr<UInputComponent> inputComponent = Preview->GetWorld()->GetFirstPlayerController<APlayerControllerX>()->InputComponent;
    inputComponent->RemoveActionBinding("Select", IE_Pressed);

    Preview->Habitat->placeBuilding(Preview);
    ConstructionSite* constructionSite = new ConstructionSite(Preview, ConstructionPlan);
    Preview->GetWorld()->GetGameInstance<UGameInstanceX>()->TheConstructionManager->AddConstruction(constructionSite);

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

void UButtonWorkaround::OnClick() {
    BuilderMode->RemoveNode(Parent);
}
