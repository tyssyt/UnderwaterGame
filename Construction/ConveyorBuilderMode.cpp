// Fill out your copyright notice in the Description page of Project Settings.

#include "ConveyorBuilderMode.h"

#include "ArrowMover.h"
#include "ConstructionSite.h"
#include "The.h"
#include "XD/CollisionProfiles.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Splitter.h"
#include "XD/Inventory/ConveyorGate.h"

FVector UConveyorBuilderMode::SourceTarget::GetLocation() const {
    check(IsValid());
     
    switch (Type) {
    case EType::Building:
        return Gate->GetComponentLocation();
    case EType::Junction:
    case EType::ConveyorLink:
    case EType::ConveyorNode:
        return Building->GetActorLocation();
    case EType::NotSet:
    default:
        checkNoEntry()
        return FVector::Zero();
    }
}

AConveyor::ESourceTargetType UConveyorBuilderMode::SourceTarget::ToConveyorType() const {    
    switch (Type) {
    case EType::Building:
    case EType::Junction:
        return AConveyor::ESourceTargetType::Building;
    case EType::ConveyorLink:
        return AConveyor::ESourceTargetType::ConveyorLink;
    case EType::ConveyorNode:
        return AConveyor::ESourceTargetType::ConveyorNode;
    default:
        checkNoEntry();
        return AConveyor::ESourceTargetType::Building;
    }
}

void UConveyorBuilderMode::SourceTarget::RemoveHighlight() {
    switch (Type) {
    case EType::NotSet:
        return;
    case EType::Building:
    case EType::Junction:
        Building->RemoveCondition(Valid ? Parent->HighlightedValid : Parent->HighlightedInvalid);
        Building = nullptr;
        Gate = nullptr;
        break;
    case EType::ConveyorNode:
        ConveyorComponent->SetVisibility(true);
        // fallthrough
    case EType::ConveyorLink:
        Building->Destroy();
        Building = nullptr;
        Conveyor = nullptr;
        ConveyorComponent = nullptr;
        break;
    default: checkNoEntry();
    }
    Valid = false;
    Type = EType::NotSet;
}
void UConveyorBuilderMode::SourceTarget::HighlightInvalid(ABuilding* building) {
    check(building);
    if (Valid == false && Type == EType::Building && Building == building)
        return;
    RemoveHighlight();

    Valid = false;
    Type = EType::Building;
    Building = building;

    Building->AddCondition(Parent->HighlightedInvalid);
}

void UConveyorBuilderMode::SourceTarget::Highlight(UConveyorGate* gate, const TArray<UConveyorGate*>& otherGates) {
    check(gate);
    if (Gate == gate)
        return;
    RemoveHighlight();

    Valid = true;
    Type = EType::Building;
    Building = gate->GetOwner<ABuilding>();
    Gate = gate;

    Building->AddCondition(Parent->HighlightedValid);

    for (const auto otherGate : otherGates) {
        if (otherGate == gate)
            continue;
        for (int i=0; i < otherGate->GetMaterials().Num(); ++i)
            otherGate->SetMaterial(i, Parent->HighlightedOption->GetMaterial());
    }
}

void UConveyorBuilderMode::SourceTarget::Highlight(AJunction* junction) {
    check(junction);
    if (Building == junction)
        return;
    RemoveHighlight();

    Valid = true;
    Type = EType::Junction;
    Building = junction;

    Building->AddCondition(Parent->HighlightedValid);
}

void UConveyorBuilderMode::SourceTarget::Highlight(ABuilding* building, AConveyor* conveyor, UConveyorNode* node, bool valid) {
    check(node);

    if (IsConveyorNode(node)) {
        // with the current calls, we check this early and never should get here
        checkNoEntry();
        return;
    }
    RemoveHighlight();

    Valid = valid;
    Type = EType::ConveyorNode;
    Building = building;
    Conveyor = conveyor;
    ConveyorComponent = node;

    Building->AddCondition(valid ? Parent->HighlightedValid : Parent->HighlightedInvalid);
    node->SetVisibility(false);
}
void UConveyorBuilderMode::SourceTarget::Highlight(ABuilding* building, AConveyor* conveyor, UConveyorLink* link, bool valid) {
    check(link);
    if (IsConveyorLink(link)) {
        check(building == Building);
        // even if we are on the same link, we still could be in a different location, so update valid
        Valid = valid;
        Building->AddCondition(valid ? Parent->HighlightedValid : Parent->HighlightedInvalid);
        return;
    }
    RemoveHighlight();

    Valid = valid;
    Type = EType::ConveyorLink;
    Building = building;
    Conveyor = conveyor;
    ConveyorComponent = link;

    Building->AddCondition(valid ? Parent->HighlightedValid : Parent->HighlightedInvalid);
}

void UConveyorBuilderMode::SourceTarget::Reset() {
    Building->RemoveCondition(Valid ? Parent->HighlightedValid : Parent->HighlightedInvalid);
    Type = EType::NotSet;
    Building = nullptr;
    Gate = nullptr;
    Conveyor = nullptr;
    ConveyorComponent = nullptr;
}

UConstructionSite* UConveyorBuilderMode::SourceTarget::CreateConstructionSite(const UEncyclopedia* encyclopedia) const {
    if (Type != EType::ConveyorNode && Type != EType::ConveyorLink)
        return nullptr;

    Conveyor->SplitAt(ConveyorComponent, Building);

    Building->RemoveConditions(Parent);
    Building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfiles::BlockAllDynamic, true);

    auto& materials = encyclopedia->Splitter->Materials;
    if (Type == EType::ConveyorNode)
        Material::AddTo(materials, encyclopedia->ConveyorNode->Materials, -1);

    return NewObject<UConstructionSite>(Building)->Init(
        Building,
        encyclopedia->Splitter->Time,
        materials,
        NewObject<UBuilderModeExtensions>()
    );
}

void UConveyorBuilderMode::SourceTarget::GetOverlapIgnore(TArray<AActor*>& allowedActors, TArray<UStaticMeshComponent*>& allowedMeshes) const {
    if (!IsValid())
        return;
    switch (Type) {
    case EType::Building:
        allowedMeshes.Add(Gate);
        break;
    case EType::Junction:
    case EType::ConveyorLink:
        allowedActors.Add(Building);
        break;
    case EType::ConveyorNode:
        allowedActors.Add(Building);
        allowedMeshes.Add(ConveyorComponent);
        break;
    case EType::NotSet:
    default:
        checkNoEntry();
    }
}

UConveyorBuilderMode* UConveyorBuilderMode::Init(UConstructionPlan* constructionPlan) {
    PreInit();

    const auto playerController = The::PlayerController(this);
    ConstructionUI->Init(constructionPlan, The::ConstructionManager(this));
    ConstructionUI->ConstructionMaterials->UpdateNeed(
        AConveyor::ComputeCosts(
            0.,
            0,
            AConveyor::ESourceTargetType::Building,
            AConveyor::ESourceTargetType::Building,
            The::Encyclopedia(this)
        ),
        The::ConstructionManager(this)
    );
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), ConstructionUI);

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

    if (!Source.IsSet()) {
        TickSelectSource(camera);
        ComputeCostSelectSource();
    } else if (!Target.IsSet()) {
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
    const auto type = CurrentHighlight.IsSet() ? CurrentHighlight.ToConveyorType() : AConveyor::ESourceTargetType::Building;
    ConstructionUI->ConstructionMaterials->UpdateNeed(
        AConveyor::ComputeCosts(0., 0, type, AConveyor::ESourceTargetType::Building, The::Encyclopedia(this)),
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

    const auto startLoc = Nodes.IsEmpty() ? Source.GetLocation() : Nodes.Last()->GetComponentLocation();
    const auto startType = Nodes.IsEmpty() ? Source.ToConveyorType() : AConveyor::ESourceTargetType::ConveyorNode;
    
    if (HighlightUnderCursor(playerController, false)) {
        if (CurrentHighlight.IsValid()) {
            FVector endLoc = CurrentHighlight.GetLocation();
            NextLink->Connect(startLoc, startType, endLoc, CurrentHighlight.ToConveyorType());
            NextLink->SetVisibility(true); 
            NextNode->SetWorldLocation(endLoc + FVector(0, 0, 5000)); // the invisible node can still cause collision, so we move it away
            NextNode->SetVisibility(false);
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
            FVector endLoc = landscapeUnderCursor.ImpactPoint + FVector(0, 0, 15);
            NextLink->Connect(startLoc, startType, endLoc, AConveyor::ESourceTargetType::ConveyorNode);
            NextLink->SetVisibility(true);            
            NextNode->SetWorldLocation(endLoc);
            NextNode->SetVisibility(true);            
        } else {
            NextNode->SetVisibility(false);
            NextLink->SetVisibility(false);
        }
    }
}

void UConveyorBuilderMode::ComputeCostSelectNextPoint() const {
    TArray<FVector> nodes;
    nodes.Add(Source.GetLocation());
    for (const auto node : Nodes)
        nodes.Add(node->GetComponentLocation());

    AConveyor::ESourceTargetType targetType;
    if (CurrentHighlight.IsValid()) {
        nodes.Add(CurrentHighlight.GetLocation());
        targetType = CurrentHighlight.ToConveyorType();
    } else {
        if (NextNode->IsVisible())
            nodes.Add(NextNode->GetComponentLocation());
        // the conveyor ends in a node, so we add a fake last node as pseudo Building for correct costs
        nodes.Emplace(nodes.Last());
        targetType = AConveyor::ESourceTargetType::Building;
    }

    ConstructionUI->ConstructionMaterials->UpdateNeed(
        AConveyor::ComputeCosts(nodes, Source.ToConveyorType(), targetType, The::Encyclopedia(this)),
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
    TArray<FVector> nodes;
    nodes.Add(Source.GetLocation());
    for (const auto node : Nodes)
        nodes.Add(node->GetComponentLocation());
    nodes.Add(Target.GetLocation());

    if (InsertNodeHoverLink) // we want to add the cost of the hovered node without increasing the link distance
        nodes.Emplace(nodes.Last());

    ConstructionUI->ConstructionMaterials->UpdateNeed(
        AConveyor::ComputeCosts(nodes, Source.ToConveyorType(), Target.ToConveyorType(), The::Encyclopedia(this)),
        The::ConstructionManager(this)
    );
}

FVector UConveyorBuilderMode::ProjectOntoLink(const FVector& loc, const UConveyorLink* link) {
    const FVector meshLocation = link->GetComponentLocation();
    const FVector projection = (loc - meshLocation).ProjectOnToNormal(link->GetComponentRotation().Vector());
    return meshLocation + projection;
}

ABuilding* UConveyorBuilderMode::SpawnJunction(bool isSource, UResource* resource) {
    ABuilding* building;
    if (isSource)
        building = GetWorld()->SpawnActor<ASplitter>();
    else
        building = GetWorld()->SpawnActor<AMerger>();
    building->AddCondition(NewObject<UInBuilderMode>(this)->WithSource(this));
    building->GetComponentByClass<UConveyorNode>()->SetCollisionProfileName(CollisionProfiles::OverlapAllDynamic, true);

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
        CurrentHighlight.RemoveHighlight();
        return false;
    }

    if (AConveyor* conveyor = Cast<AConveyor>(hitResult.GetActor())) {
        if (UConveyorNode* node = Cast<UConveyorNode>(hitResult.GetComponent()))
            return HighlightConveyorNodeUnderCursor(conveyor, node, isSource);
        if (UConveyorLink* link = Cast<UConveyorLink>(hitResult.GetComponent()))
            return HighlightConveyorLinkUnderCursor(conveyor, link, hitResult.ImpactPoint, isSource);
 
        checkNoEntry();
        return false;
    }

    if (ABuilding* building = Cast<ABuilding>(hitResult.GetActor()))
        return HighlightBuildingUnderCursor(building, hitResult.ImpactPoint, isSource);

    CurrentHighlight.RemoveHighlight();
    return false;
}

bool UConveyorBuilderMode::HighlightConveyorNodeUnderCursor(AConveyor* conveyor, UConveyorNode* node, bool isSource) {
    if (CurrentHighlight.IsConveyorNode(node))
        return true; // already highlighted

    const auto splitter = SpawnJunction(isSource, conveyor->SourceInv->Resource);
    splitter->SetActorLocation(node->GetComponentLocation());

    bool valid = CheckValidBuilding(splitter, isSource);
    // TODO The current splitting on construction cannot handle 2 splits in the same conveyor, so we just forbid it here
    valid &= isSource || Source.GetConveyor() != conveyor;
    CurrentHighlight.Highlight(splitter, conveyor, node, valid);
    return true;
}


bool UConveyorBuilderMode::HighlightConveyorLinkUnderCursor(AConveyor* conveyor, UConveyorLink* link, const FVector& hitLoc, bool isSource) {
    const auto splitter = CurrentHighlight.IsConveyorLink(link) ? CurrentHighlight.GetBuilding() : SpawnJunction(isSource, conveyor->SourceInv->Resource);
    splitter->SetActorLocation(ProjectOntoLink(hitLoc, link));

    bool valid = CheckValidBuilding(splitter, isSource);
    // TODO The current splitting on construction cannot handle 2 splits in the same conveyor, so we just forbid it here
    valid &= isSource || Source.GetConveyor() != conveyor;

    if (isSource)
        valid &= !CheckOverlap(splitter->GetComponentByClass<UStaticMeshComponent>(), {link}, {});
    else
        valid &= !CheckOverlap(splitter->GetComponentByClass<UStaticMeshComponent>(), {link, NextLink, NextNode}, {});

    CurrentHighlight.Highlight(splitter, conveyor, link, valid);
    return true;
}

bool UConveyorBuilderMode::HighlightBuildingUnderCursor(ABuilding* building, const FVector& hitLoc, bool isSource) {
    if (const auto junction = Cast<AJunction>(building)) { // junctions don't have conveyor gates
        if (CheckValidBuilding(junction, isSource)) {            
            CurrentHighlight.Highlight(junction);
            return true;
        } else {
            CurrentHighlight.HighlightInvalid(junction);
            return false;
        }
    }

    TArray<UConveyorGate*> gates;
    building->GetComponents<UConveyorGate>(gates);
    gates.RemoveAllSwap([isSource](const UConveyorGate* it){return it->Conveyor != nullptr || isSource == it->IsInput();});

    double nearestDist = INFINITY;
    UConveyorGate* nearestGate = nullptr;
    for (const auto gate : gates) {
        const double dist = FVector::Distance(gate->GetComponentLocation(), hitLoc);
        if (dist < nearestDist) {
            nearestDist = dist;
            nearestGate = gate;
        }
    }

    if (nearestGate != nullptr && CheckValidBuilding(building, isSource)) {
        CurrentHighlight.Highlight(nearestGate, gates);
        return true;
    } else {
        CurrentHighlight.HighlightInvalid(building);
        return false;
    }
}

void UConveyorBuilderMode::CheckOverlap() {
    HasOverlap = CheckOverlapLinks() | CheckOverlapNodes(); // we need both sides to always run, so just a single |
    
    if (ConfirmSymbol)
        ConfirmSymbol->SetVisibility(!HasOverlap);
}
bool UConveyorBuilderMode::CheckOverlapLinks() {
    if (Target.IsSet() && Links.Num() == 1) {
        // special case for 1 segment conveyor
        return CheckOverlap(Links[0], {}, {&Source, &Target});
    }

    bool hasOverlapLinks = false;
    if (Links.Num() > 0) {
        // The first Link is allowed to overlap the Source and the first Node
        hasOverlapLinks |= CheckOverlap(Links[0], {Nodes[0]}, {&Source});
    }
    const int numNormalLink = Target.IsSet() ? Links.Num() - 1 : Links.Num();
    for (int i=1; i < numNormalLink; ++i) {
        // Normal Links may overlap the Nodes before and after them
        hasOverlapLinks |= CheckOverlap(Links[i], {Nodes[i-1], Nodes[i]}, {});
    }
    if (Target.IsSet() && Links.Num() > 1) {
        // if the Target is set, the last Link may overlap the Target and the last Node
        hasOverlapLinks |= CheckOverlap(Links.Last(), {Nodes.Last()}, {&Target}); 
    }

    if (!Target.IsSet() && NextLink->IsVisible()) {
        TArray<UStaticMeshComponent*> allowedMeshes;
        TArray<SourceTarget*> allowed;

        // Next Link may overlap the last Node, if there are no Nodes it may overlap the Source
        if (Nodes.Num() > 0)
            allowedMeshes.Add(Nodes.Last());
        else
            allowed.Add(&Source);

        if (CurrentHighlight.IsSet())
            allowed.Add(&CurrentHighlight);

        if (NextNode->IsVisible())
            allowedMeshes.Add(NextNode);
        
        hasOverlapLinks |= CheckOverlap(NextLink, allowedMeshes, allowed);
    }

    return hasOverlapLinks;
}
bool UConveyorBuilderMode::CheckOverlapNodes() {
    bool hasOverlapNodes = false;
    if (Nodes.Num() > 0) {
        for (int i=0; i < Nodes.Num()-1; ++i) {
            // Normal Nodes may overlap the link before and after them
            hasOverlapNodes |= CheckOverlap(Nodes[i], {Links[i], Links[i+1]}, {});
        }
        if (Target.IsSet()) {
            // if the Target is set, the last nodes behaves like a normal Node
            hasOverlapNodes |= CheckOverlap(Nodes.Last(), {Links[Links.Num()-2], Links.Last()}, {});
        } else {
            // otherwise, the last Node may overlap NextLink
            hasOverlapNodes |= CheckOverlap(Nodes.Last(), {Links.Last(), NextLink}, {});
        }
    }

    if (!Target.IsSet() && NextNode->IsVisible()) {
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
    for (const auto a : allowed)
        a->GetOverlapIgnore(allowedActors, allowedNodes);

    bool hasOverlap = false;        
    for (const auto overlap : overlaps) {
        if (allowedMeshes.Contains(overlap) || allowedNodes.Contains(overlap) || allowedActors.Contains(overlap->GetAttachmentRootActor()))
            continue;
        hasOverlap = true;
        UE_LOG(LogTemp, Error, TEXT("%s on %s overlaps with %s on %s"), *mesh->GetName(), *mesh->GetOwner()->GetName(), *overlap->GetName(), *overlap->GetOwner()->GetName());
        break;
    }

    if (hasOverlap)
        for (int i=0; i < mesh->GetMaterials().Num(); ++i)
            mesh->SetMaterial(i, HighlightedInvalid->GetMaterial());
    else
        for (int i=0; i < mesh->GetMaterials().Num(); ++i)
            mesh->SetMaterial(i, nullptr);

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
    if (!Source.IsSet()) {
        if (!CurrentHighlight.IsValid())
            return;

        Source = CurrentHighlight;
        CurrentHighlight.Reset();

        Preview = GetWorld()->SpawnActor<AActor>();
        Preview->SetActorTickEnabled(false);
        Preview->SetRootComponent(NewObject<USceneComponent>(Preview));
        CreateNextLinkAndNode();
        return;
    }

    if (!Target.IsSet()) {
        if (!CurrentHighlight.IsSet()) {
            if (NextNode->IsVisible() && NextLink->IsVisible()) {
                // create new Node
                Nodes.Push(NextNode);
                Links.Push(NextLink);
                CreateNextLinkAndNode();
            }
            return;
        }

        if (!CurrentHighlight.IsValid())
            return;

        // find common resource
        const auto resources = AConveyor::FindCommonResources(Source.GetBuilding()->FindComponentByClass<UInventoryComponent>(), CurrentHighlight.GetBuilding()->FindComponentByClass<UInventoryComponent>());
        if (resources.IsEmpty()) {
            checkNoEntry();
            return;
        }
        if (resources.Num() > 1) {
            UE_LOG(LogTemp, Error, TEXT("Multiple Matches found"));
            return; // TODO show a dialog to select the input
            // TODO if the input/target is untyped, and there are multiple outputs/sources, also prompt
            // TODO think about if we want the vice versa of the above
            // TODO think about depot to depot connections... if neither of them is connected to something else in the chain...
        }
        Resource = *resources.begin();


        Target = CurrentHighlight;
        CurrentHighlight.Reset();

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
        ConfirmSymbol->SetWorldLocation(Target.GetBuilding()->GetActorLocation());
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
        cancelSymbol->SetWorldLocation(Target.GetBuilding()->GetActorLocation());
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

void UConveyorBuilderMode::UpdateLinks(UConveyorNode* node) {
    const auto idx = Nodes.Find(node);
    check(idx != INDEX_NONE);
    const FVector nodeLoc = node->GetComponentLocation();

    // update link before Node
    {
        UConveyorLink* link = Links[idx];
        if (idx <= 0)
            link->Connect(Source.GetLocation(), Source.ToConveyorType(), nodeLoc, AConveyor::ESourceTargetType::ConveyorNode);
        else
            link->Connect(Nodes[idx-1]->GetComponentLocation(), AConveyor::ESourceTargetType::ConveyorNode, nodeLoc, AConveyor::ESourceTargetType::ConveyorNode);
    }    

    // update link after Node
    {
        UConveyorLink* link = Links[idx+1];
        if (idx >= Nodes.Num()-1)
            link->Connect(nodeLoc, AConveyor::ESourceTargetType::ConveyorNode, Target.GetLocation(), Target.ToConveyorType());
        else
            link->Connect(nodeLoc, AConveyor::ESourceTargetType::ConveyorNode, Nodes[idx+1]->GetComponentLocation(), AConveyor::ESourceTargetType::ConveyorNode);
    }
    CheckOverlap();
}

void UConveyorBuilderMode::OnClickConfirm() {
    if (HasOverlap || Done)
        return;

    const auto encyclopedia = The::Encyclopedia(this);
    // Splitter
    if (const auto constructionSite = Source.CreateConstructionSite(encyclopedia))
        constructionSite->QueueTasks();
    // Merger
    if (const auto constructionSite = Target.CreateConstructionSite(encyclopedia))
        constructionSite->QueueTasks();     

    // Conveyor
    TArray<FVector> nodes;
    nodes.Add(Source.GetLocation());
    for (const auto& node : Nodes)
        nodes.Add(node->GetComponentLocation());
    nodes.Add(Target.GetLocation());

    const auto conveyor = AConveyor::Create(
        GetWorld(),
        Source.GetBuilding(),
        Source.GetGate(),
        Target.GetBuilding(),
        Target.GetGate(),
        nodes,
        Resource
    );
    NewObject<UConstructionSite>(conveyor)->Init(
        conveyor,
        1, // TODO make time scale with length, or better do a cool building animation where the ship flies along the conveyor
        AConveyor::ComputeCosts(nodes, AConveyor::ESourceTargetType::Building, AConveyor::ESourceTargetType::Building, encyclopedia),
        NewObject<UBuilderModeExtensions>()
    )->QueueTasks(); 

    Stop(false);
}

void UConveyorBuilderMode::OnClickCancel() {
    Stop(true);
}

bool UConveyorBuilderMode::CheckValidBuilding(const ABuilding* building, const bool isSource) const {
    const auto inventory = building->GetComponentByClass<UInventoryComponent>();
    if (!inventory)
        return false;

    if (isSource)
        return inventory->GetUnconnected(false).Num() > 0;
    else
        return AConveyor::HasCommonResource(Source.GetBuilding()->GetComponentByClass<UInventoryComponent>(), inventory);
}

UClass* UConveyorBuilderMode::IDK() {
    return AConveyor::StaticClass();
}

void UConveyorBuilderMode::Stop(bool cancelled) {
    if (Done)
        return;
    Done = true;

    if (cancelled) {
        Source.RemoveHighlight();
        Target.RemoveHighlight();
    }

    CurrentHighlight.RemoveHighlight();

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
        Links[0]->Connect(Source.GetLocation(), Source.ToConveyorType(), Target.GetLocation(), Target.ToConveyorType());
        CheckOverlap();
    }
}

void UButtonWorkaround::OnClick() {
    BuilderMode->RemoveNode(Parent);
}