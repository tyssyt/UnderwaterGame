// Fill out your copyright notice in the Description page of Project Settings.

#include "ConveyorBuilderMode.h"

#include <optional>

#include "ArrowMover.h"
#include "BuilderModeExtension.h"
#include "XD/CollisionProfiles.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Conveyor.h"
#include "XD/GameInstanceX.h"
#include "XD/Buildings/Splitter.h"

#include "Components/WidgetComponent.h"
#include "XD/Utils.h"

AConveyor::ESourceTargetType ToESourceTargetType(UConveyorBuilderMode::SourceTarget::EType type) {
    switch (type) {
    case UConveyorBuilderMode::SourceTarget::EType::Building:
        return AConveyor::ESourceTargetType::Building;
    case UConveyorBuilderMode::SourceTarget::EType::ConveyorLink:
        return AConveyor::ESourceTargetType::ConveyorLink;
    case UConveyorBuilderMode::SourceTarget::EType::ConveyorNode:
        return AConveyor::ESourceTargetType::ConveyorNode;
    case UConveyorBuilderMode::SourceTarget::EType::NotSet:
    default:
        checkNoEntry();
        return AConveyor::ESourceTargetType::Building;
    }
}

UConveyorBuilderMode::UConveyorBuilderMode() {    
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> RedMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/BuilderMode_NotBuildable"));
    RedMaterial = RedMaterialFinder.Object; 
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> GreenMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/BuilderMode_Buildable"));
    GreenMaterial = GreenMaterialFinder.Object;
    const static ConstructorHelpers::FObjectFinder<UTexture2D> ConveyorImageFinder(TEXT("/Game/Assets/Resources/Placeholder")); // TODO Conveyor Image
    ConveyorImage = ConveyorImageFinder.Object;
}

UConveyorBuilderMode* UConveyorBuilderMode::Init(UConstructionPlan* constructionPlan) { // TODO currently that plan is ConveyorLink, once it is Conveyor we can use it to fill stuff below
    const auto playerController = The::PlayerController(this);

    static FText conveyorName = FText::FromString(TEXT("Conveyor"));
    playerController->BlueprintHolder->ConstructionUI->Set(
        conveyorName,
        ConveyorImage,
        AConveyor::ComputeCosts(0., 0, AConveyor::ESourceTargetType::Building, AConveyor::ESourceTargetType::Building, The::Encyclopedia(this)),
        The::ConstructionManager(this)
    );
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), playerController->BlueprintHolder->ConstructionUI);
    
    // bind keys
    const auto inputComponent = playerController->InputComponent;
    inputComponent->BindAction("Select", IE_Pressed, this, &UConveyorBuilderMode::Confirm);
    return this;
}

void UConveyorBuilderMode::CreateNextLinkAndNode() {
    USceneComponent* sceneComponent = Preview->GetRootComponent();
    NextLink = NewObject<UConveyorLink>(Preview);
    NextLink->RegisterComponent();
    NextLink->AttachToComponent(sceneComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    NextLink->SetCollisionProfileName(CollisionProfiles::OverlapAllDynamic, true);
    Preview->AddInstanceComponent(NextLink);

    NextNode = NewObject<UConveyorNode>(Preview);
    NextNode->RegisterComponent();
    NextNode->AttachToComponent(sceneComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    NextNode->SetCollisionProfileName(CollisionProfiles::OverlapAllDynamic, true);
    Preview->AddInstanceComponent(NextNode);
}

bool UConveyorBuilderMode::Tick(const ACameraPawn& camera) {
    if (Done)
        return true;
    
    if (Source.Type == SourceTarget::EType::NotSet) {
        TickSelectSource(camera);
        ComputeCostSelectSource();
    } else if (Target.Type == SourceTarget::EType::NotSet) {
        TickSelectNextPoint(camera);
        ComputeCostSelectNextPoint();
        CheckOverlap();
    } else {
        TickInsertNode(camera);
        ComputeCostInsertNode();
    }
    return false;
}

void UConveyorBuilderMode::TickSelectSource(const ACameraPawn& camera) {
    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController)
        return;
    
    HighlightUnderCursor(playerController, true);
}

void UConveyorBuilderMode::ComputeCostSelectSource() const {
    const auto splitter = CurrentHighlightValid ? ToESourceTargetType(CurrentHighlight.Type) : AConveyor::ESourceTargetType::Building;
    The::BPHolder(this)->ConstructionUI->Set(
        AConveyor::ComputeCosts(0., 0, splitter, AConveyor::ESourceTargetType::Building, The::Encyclopedia(this)),
        The::ConstructionManager(this)
    );
}

void UConveyorBuilderMode::TickSelectNextPoint(const ACameraPawn& camera) {
    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
    if (!playerController) {
        NextNode->SetVisibility(false);
        NextLink->SetVisibility(false);
        return;
    }
    
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

void UConveyorBuilderMode::ComputeCostSelectNextPoint() const {    
    TArray<FVector> nodes;
    for (const auto node : Nodes)
        nodes.Add(node->GetComponentLocation());

    TOptional<FVector> end;
    auto merger = AConveyor::ESourceTargetType::Building;
    if (NextNode->IsVisible()) {
        nodes.Add(NextNode->GetComponentLocation());
    } else if (CurrentHighlightValid) {
        end = CurrentHighlight.Building->GetActorLocation();
        merger = ToESourceTargetType(CurrentHighlight.Type);
    } else
        check(!NextLink->IsVisible());

    The::BPHolder(this)->ConstructionUI->Set(
        AConveyor::ComputeCosts(
            Source.Building->GetActorLocation(),
            end.IsSet() ? &*end : nullptr,
            nodes,
            ToESourceTargetType(Source.Type),
            merger,
            The::Encyclopedia(this)),
        The::ConstructionManager(this)
    );
}

void UConveyorBuilderMode::TickInsertNode(const ACameraPawn& camera) {
    // Do not show new Node Preview if any Mover is active
    TInlineComponentArray<UArrowMover*> movers;
    Preview->GetComponents<UArrowMover>(movers, true);
    for (const UArrowMover* mover : movers) {
        if (mover->IsComponentTickEnabled()) {
            NextNode->SetVisibility(false);
            InsertNodeHoverLink = nullptr;
            // the invisible node can still cause collision, so we move it away
            NextNode->SetWorldLocation(camera.GetActorLocation() + FVector(0, 0, 5000));
            return;
        }
    }

    // Show new Node Preview if the Mouse is over a Link
    const APlayerControllerX* playerController = camera.GetController<APlayerControllerX>();
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

void UConveyorBuilderMode::ComputeCostInsertNode() const {
    const FVector end = Target.Building->GetActorLocation();

    TArray<FVector> nodes;
    for (const auto node : Nodes)
        nodes.Add(node->GetComponentLocation());
    if (InsertNodeHoverLink) // we want to add the cost of one conveyor node without increasing the link distance
        nodes.Add(end);

    The::BPHolder(this)->ConstructionUI->Set(
        AConveyor::ComputeCosts(
            Source.Building->GetActorLocation(),
            &end,
            nodes,
            ToESourceTargetType(Source.Type),
            ToESourceTargetType(Target.Type),
            The::Encyclopedia(this)),
        The::ConstructionManager(this)
    );
}

FVector UConveyorBuilderMode::ProjectOntoLink(FVector loc, const UConveyorLink* link) {
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
        building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfiles::OverlapAllDynamic, true);
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
        TArray<UStaticMeshComponent*> allowedMeshes;
        TArray<SourceTarget*> allowed;

        // Next Link may overlap the last Node, if there are no Nodes it may overlap the Source
        if (Nodes.Num() > 0)
            allowedMeshes.Add(Nodes.Last());
        else
            allowed.Add(&Source);

        if (CurrentHighlight.Type != SourceTarget::EType::NotSet)
            allowed.Add(&CurrentHighlight);

        if (NextNode->IsVisible())
            allowedMeshes.Add(NextNode);
        
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

bool UConveyorBuilderMode::CheckOverlap(UStaticMeshComponent* mesh, const TArray<UStaticMeshComponent*>& allowedMeshes, const TArray<SourceTarget*>& allowed) const {
    mesh->UpdateOverlaps(nullptr, false);
    TSet<UPrimitiveComponent*> overlaps;
    mesh->GetOverlappingComponents(overlaps);

    TArray<AActor*> allowedActors;
    TArray<UStaticMeshComponent*> allowedNodes;
    for (const auto a : allowed) {
        allowedActors.Add(a->Building);
        if (a->Type == SourceTarget::EType::ConveyorNode)
            allowedNodes.Add(a->ConveyorComponent);
    }
        
    bool hasOverlap = false;        
    for (const auto overlap : overlaps) {
        if (allowedMeshes.Contains(overlap) || allowedNodes.Contains(overlap) || allowedActors.Contains(overlap->GetAttachmentRootActor()))
            continue;
        hasOverlap = true;
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
    const auto playerController = The::PlayerController(this);
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
        for (UStaticMeshComponent* link : Links)          
            link->SetCollisionProfileName(CollisionProfiles::UI, true);

        UTexture2D* cancelTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Cancel"));
        // create arrows over all Nodes
        for (const auto node : Nodes)
            AddArrowsToNode(node, cancelTexture);  

        // create confirm / cancel over target
        const auto playerController = The::PlayerController(this);
        
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
        const auto idx = Links.Find(InsertNodeHoverLink);
        check(idx != INDEX_NONE);

        UConveyorNode* insertNode = NextNode;
        AddArrowsToNode(insertNode, LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Cancel")));
        if (idx < Nodes.Num())
            Nodes.Insert(insertNode, idx);
        else
            Nodes.Push(insertNode);

        CreateNextLinkAndNode();
        
        NextLink->SetCollisionProfileName(CollisionProfiles::UI, true);
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
    CurrentHighlightValid = false;
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
    if (HasOverlap || Done)
        return;

    const auto encyclopedia = The::Encyclopedia(this);
    const auto constructionManager = The::ConstructionManager(this);

    // Splitter
    if (Source.Type == SourceTarget::EType::ConveyorNode || Source.Type == SourceTarget::EType::ConveyorLink) {
        Source.Building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfiles::BlockAllDynamic, true);

        TArray<Material> material = encyclopedia->Splitter->Materials;
        if (Source.Type == SourceTarget::EType::ConveyorNode)
            Material::AddTo(material, encyclopedia->ConveyorNode->Materials, -1);

        ConstructionSite* constructionSite = new ConstructionSite(Source.Building, encyclopedia->Splitter->Time, material, FConstructionFlags{false});
        constructionManager->AddConstruction(constructionSite);

        // split the conveyor
        Source.Conveyor->SplitAt(Source.ConveyorComponent, Source.Building);
    }
            
    // Conveyor
    {
        TArray<FVector> nodes;
        nodes.Reserve(Nodes.Num());
        for (const auto& node : Nodes)
            nodes.Push(node->GetComponentLocation());
        AConveyor* conveyor = AConveyor::Create(GetWorld(), Source.Building, Target.Building, nodes, Resource);
        FVector targetLoc = Target.Building->GetActorLocation();
        ConstructionSite* constructionSite = new ConstructionSite(
            conveyor,
            1, // TODO make time scale with length, or better do a cool building animation where the ship flies along the conveyor
            AConveyor::ComputeCosts(Source.Building->GetActorLocation(), &targetLoc, nodes, AConveyor::ESourceTargetType::Building, AConveyor::ESourceTargetType::Building, encyclopedia),
            FConstructionFlags{false}
        ); 
        constructionManager->AddConstruction(constructionSite);
    }
    
    // Merger
    if (Target.Type == SourceTarget::EType::ConveyorNode || Target.Type == SourceTarget::EType::ConveyorLink) {
        Target.Building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfiles::BlockAllDynamic, true);

        TArray<Material> material = encyclopedia->Merger->Materials;
        if (Target.Type == SourceTarget::EType::ConveyorNode)
            Material::AddTo(material, encyclopedia->ConveyorNode->Materials, -1);

        ConstructionSite* constructionSite = new ConstructionSite(Target.Building, encyclopedia->Merger->Time, material, FConstructionFlags{false});
        constructionManager->AddConstruction(constructionSite);

        // split the conveyor
        Target.Conveyor->SplitAt(Target.ConveyorComponent, Target.Building);
    }
    
    Stop(true);
}

void UConveyorBuilderMode::OnClickCancel() {
    Stop(false);
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

void UConveyorBuilderMode::Stop(bool success) {
    if (Done)
        return;
    Done = true;

    if (!success) {
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
    }
    
    RemoveCurrentHighlight();

    if (Preview) {
        Preview->Destroy();
        Preview = nullptr;
    }

    const auto playerController = The::PlayerController(this);
    playerController->Deselect();

    // unbind
    playerController->InputComponent->RemoveActionBinding("Select", IE_Pressed);
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

void UButtonWorkaround::OnClick() {
    BuilderMode->RemoveNode(Parent);
}