// Fill out your copyright notice in the Description page of Project Settings.

#include "Conveyor.h"

#include "Splitter.h"
#include "XD/Inventory/ConveyorGate.h"

AConveyor* AConveyor::Create(
    UWorld* world,
    ABuilding* source,
    UConveyorGate* sourceGate,
    ABuilding* target,
    UConveyorGate* targetGate,
    const TArray<FVector>& nodes,
    UResource* resource
) {
    AConveyor* conveyor = world->SpawnActor<AConveyor>();
    conveyor->Source = source;
    conveyor->SourceGate = sourceGate;
    conveyor->Target = target;
    conveyor->TargetGate = targetGate;
    conveyor->Nodes = nodes;
    conveyor->Connect(resource);
    return conveyor;
}

AConveyor::AConveyor() {
    PrimaryActorTick.bCanEverTick = true;    
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));    
}

void AConveyor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (TargetInv) {
        // TODO only enable Tick if Source & Target Exists
        CurrentThroughput = TargetInv->PushInto(Buffer);
        Buffer -= CurrentThroughput;
    }

    if (SourceInv) {
        // TODO only enable Tick if Source & Target Exists
        Buffer += SourceInv->PullFrom(MaxThroughput - Buffer);
    }
}

void AConveyor::Connect(UResource* resource) {
    if (!Source || !Target || Source == Target) {
        UE_LOG(LogTemp, Error, TEXT("Invalid Connect on %s"), *this->GetName());
        return;
    }
    
    if (!resource) {
        const auto resources = FindCommonResources(Source->GetComponentByClass<UInventoryComponent>(), Target->GetComponentByClass<UInventoryComponent>());
        if (resources.Num() != 1)
            return;
        resource = *resources.begin();
    }
    
    SourceInv = nullptr;
    TargetInv = nullptr;

    // TODO extract common code from hande Target & handle Source
    
    // handle Source
    UInventoryComponent* sourceInventory = Source->FindComponentByClass<UInventoryComponent>();
    if (!sourceInventory)
        return;
    auto outputs = sourceInventory->GetUnconnected(false);
    for (const auto output : outputs) {
        if (output->Resource == resource) {
            SourceInv = output;
            break;
        }
        if (!output->Resource)
            SourceInv = output;
    }
    check(SourceInv != nullptr);

    if (!SourceInv->Resource) // we found a untyped input, type it
        SourceInv->Resource = resource;
    sourceInventory->SetConveyor(false, SourceInv, this);
    if (ASplitter* splitter = Cast<ASplitter>(Source))
        splitter->Connections++;

    // handle Target
    UInventoryComponent* targetInventory = Target->FindComponentByClass<UInventoryComponent>();
    if (!targetInventory)
        return;
    auto inputs = targetInventory->GetUnconnected(true);
    for (const auto input : inputs) {
        if (input->Resource == resource) {
            TargetInv = input;
            break;
        }
        if (!input->Resource) {
            TargetInv = input;
        }
    }
    check(TargetInv != nullptr);

    if (!TargetInv->Resource) // we found a untyped input, type it
        TargetInv->Resource = resource;
    targetInventory->SetConveyor(true, TargetInv, this);
    if (AMerger* merger = Cast<AMerger>(Target))
        merger->Connections++;

    if (SourceGate)
        SourceGate->Conveyor = this;
    if (TargetGate)
        TargetGate->Conveyor = this;

    // Visually Connect Source to Target via Nodes
    SetActorLocation((Source->GetActorLocation() + Target->GetActorLocation()) * .5f);

    ESourceTargetType startType;
    if (Source->IsA<ASplitter>() || Source->IsA<AMerger>())
        startType = ESourceTargetType::ConveyorNode;
    else
        startType = ESourceTargetType::Building;
    
    ESourceTargetType endType;
    if (Target->IsA<ASplitter>() || Target->IsA<AMerger>())
        endType = ESourceTargetType::ConveyorNode;
    else
        endType = ESourceTargetType::Building;

    if (Nodes.Num() == 2)
        MakeLink(Nodes[0], startType, Nodes[1], endType);
    else {
        MakeLink(Nodes[0], startType, Nodes[1], ESourceTargetType::ConveyorNode);
        for (int idx=1; idx < Nodes.Num()-2; idx++) {
            MakeNode(Nodes[idx]);
            MakeLink(Nodes[idx], ESourceTargetType::ConveyorNode, Nodes[idx+1], ESourceTargetType::ConveyorNode);
        }
        MakeNode(Nodes.Last(1));
        MakeLink(Nodes.Last(1), ESourceTargetType::ConveyorNode, Nodes.Last(), endType);
    }
}

void AConveyor::OnDismantle() {
    Super::OnDismantle();

    if (Source->IsA<ASplitter>() || Source->IsA<AMerger>())
        checkNoEntry(); // TODO
    Source->FindComponentByClass<UInventoryComponent>()->SetConveyor(false, SourceInv, nullptr);
    SourceGate->Conveyor = nullptr;
    Source = nullptr;
    SourceInv = nullptr;
    SourceGate = nullptr;

    if (Target->IsA<ASplitter>() || Target->IsA<AMerger>())
        checkNoEntry(); // TODO
    Target->FindComponentByClass<UInventoryComponent>()->SetConveyor(true, TargetInv, nullptr);
    TargetGate->Conveyor = nullptr;
    Target = nullptr;
    TargetInv = nullptr;
    TargetGate = nullptr;
}

void AConveyor::MakeNode(const FVector& location) {
    UConveyorNode* node = NewObject<UConveyorNode>(this);
    node->RegisterComponent();
    node->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    node->SetWorldLocation(location);
    this->AddInstanceComponent(node);    
}

void AConveyor::MakeLink(
    const FVector& start,
    ESourceTargetType startType,
    const FVector& end,
    ESourceTargetType endType
) {
    UConveyorLink* nextLink = NewObject<UConveyorLink>(this);
    nextLink->RegisterComponent();
    nextLink->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    this->AddInstanceComponent(nextLink);
    nextLink->Connect(start, startType, end, endType);
}

bool AConveyor::HasCommonResource(UInventoryComponent* source, UInventoryComponent* target) {
    return FindCommonResources(source, target).Num() > 0;
}

TSet<UResource*> AConveyor::FindCommonResources(UInventoryComponent* source, UInventoryComponent* target) {
    if (!source || !target || source == target || source->GetOutputs().Num() == 0 || target->GetInputs().Num() == 0)
        return {};

    auto inputs = target->GetUnconnected(true);
    auto outputs = source->GetUnconnected(false);
    
    // find matching in/outputs
    TSet<UResource*> resources;
    bool hasUntypedInput = false;
    bool hasUntypedOutput = false;
    for (const auto output : outputs) {
        if (!output->Resource) {
            hasUntypedOutput = true;
            continue;
        }
        for (const auto input : inputs) {
            if (!input->Resource)
                hasUntypedInput = true;
            if (input->Resource == output->Resource)
                resources.Add(input->Resource);
        }
    }
    
    if (resources.IsEmpty()) { // no matches found, check for untyped in- or outputs

        if (hasUntypedInput) {
            resources.Empty();
            for (const auto output : outputs)
                resources.Add(output->Resource);

            if (resources.Num() == 1)
                return {*resources.begin()};
        }

        if (hasUntypedOutput) {
            resources.Empty();
            for (const auto input : inputs)
                resources.Add(input->Resource);

            if (resources.Num() == 1)
                return {*resources.begin()};
        }
        
        if (hasUntypedInput ^ hasUntypedOutput)
            UE_LOG(LogTemp, Error, TEXT("No Matches found, even though there is an Untyped Input/Output"));
        return {};
    }
    if (resources.Num() == 1) // exactly one match found, connect them
        return {*resources.begin()};

    return MoveTemp(resources);
}

TArray<Material> AConveyor::ComputeCosts(const TArray<FVector>& nodes, ESourceTargetType splitter, ESourceTargetType merger, const UEncyclopedia* theEncyclopedia) {
    double linkDist = 0.;
    for (int idx=0; idx<nodes.Num()-1; idx++)
        linkDist += FVector::Distance(nodes[idx], nodes[idx+1]);
    return ComputeCosts(linkDist, FMath::Max(nodes.Num()-2, 0), splitter, merger, theEncyclopedia);
}

inline static constexpr double ConveyorLinkDistanceScale = 13.37; // TODO this needs to be explained somewhere in the encyclopedia
TArray<Material> AConveyor::ComputeCosts(double linkDist, int numNodes, ESourceTargetType splitter, ESourceTargetType merger, const UEncyclopedia* theEncyclopedia) {
    TArray<Material> materials;
    Material::AddTo(materials, theEncyclopedia->ConveyorLink->Materials, static_cast<int>(linkDist / ConveyorLinkDistanceScale));
    Material::AddTo(materials, theEncyclopedia->ConveyorNode->Materials, numNodes);

    switch (splitter) {
    case ESourceTargetType::Building:
        break;
    case ESourceTargetType::ConveyorNode:
        Material::AddTo(materials, theEncyclopedia->ConveyorNode->Materials, -1);
        // fallthrough
    case ESourceTargetType::ConveyorLink:
        Material::AddTo(materials, theEncyclopedia->Splitter->Materials);
        break;
    default:
        checkNoEntry();
    }

    switch (merger) {
    case ESourceTargetType::Building:
        break;
    case ESourceTargetType::ConveyorNode:
        Material::AddTo(materials, theEncyclopedia->ConveyorNode->Materials, -1);
        // fallthrough
    case ESourceTargetType::ConveyorLink:
        Material::AddTo(materials, theEncyclopedia->Merger->Materials);
        break;
    default:
        checkNoEntry();
    }

    return materials;
}

TPair<AConveyor*, AConveyor*> AConveyor::SplitAt(UStaticMeshComponent* mesh, ABuilding* building) {
    TInlineComponentArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes);
    check(meshes.Contains(mesh));

    TArray<FVector> nodes1;
    TArray<FVector> nodes2;
    nodes1.Add(Nodes[0]);
    nodes2.Add(building->GetActorLocation());

    TArray<FVector>* cur = &nodes1;
    for (const auto m : meshes) {
        if (m == mesh) {
            cur = &nodes2;
            continue;
        }
        if (m->IsA<UConveyorNode>())
            cur->Push(m->GetComponentLocation());
    }
    nodes1.Add(building->GetActorLocation());
    nodes2.Add(Nodes.Last());

    // TODO this should probably go to some generic onDelete
    if (const auto splitter = Cast<ASplitter>(Source))
        splitter->Disconnect(this);
    else
        Source->FindComponentByClass<UInventoryComponent>()->SetConveyor(false, SourceInv, nullptr);

    if (const auto merger = Cast<AMerger>(Target))
        merger->Disconnect(this);
    else
        Target->FindComponentByClass<UInventoryComponent>()->SetConveyor(true, TargetInv, nullptr);
    
    this->Destroy();
    return MakeTuple(
        Create(GetWorld(), Source, SourceGate, building, nullptr, nodes1, SourceInv->Resource),
        Create(GetWorld(), building, nullptr, Target, TargetGate, nodes2, SourceInv->Resource)
    );
}

UConveyorLink::UConveyorLink() {    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Conveyor/ConveyorLink"));
    UStaticMeshComponent::SetStaticMesh(MeshFinder.Object);
    SetRenderCustomDepth(true);
}

void UConveyorLink::Connect(
    FVector start,
    AConveyor::ESourceTargetType startType,
    FVector end,
    AConveyor::ESourceTargetType endType
) {     
    FVector startToEnd = end - start;

    // Nudge start end end a little bit towards each other, not changing the z component
    FVector nudge = FVector(startToEnd.X, startToEnd.Y, 0.);
    if (nudge.Normalize()) {
        switch (startType) {
        case AConveyor::ESourceTargetType::Building:
            start = start + 1 * nudge;
            break;
        case AConveyor::ESourceTargetType::ConveyorNode:
        case AConveyor::ESourceTargetType::ConveyorLink:
            start = start + 13 * nudge;
            break;
        default:
            checkNoEntry();
        }
        switch (endType) {
        case AConveyor::ESourceTargetType::Building:
            end = end - 1 * nudge;
            break;
        case AConveyor::ESourceTargetType::ConveyorNode:
        case AConveyor::ESourceTargetType::ConveyorLink:
            end = end - 13 * nudge;
            break;
        default:
            checkNoEntry();
        }
        startToEnd = end - start;
    }
    
    SetWorldLocation((start + end) * .5f);
    SetWorldRotation(startToEnd.Rotation());
    SetWorldScale3D(FVector(startToEnd.Size() / 100.f, .1f, 0.1f));
}

UConveyorNode::UConveyorNode() {    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    UStaticMeshComponent::SetStaticMesh(MeshFinder.Object);
    SetRenderCustomDepth(true);
    SetWorldScale3D(FVector(.3, .3, .1));
}

void UConveyorUI::Tick() {
    if (Conveyor) {
        CurrentThroughput->SetText(FText::AsNumber(Conveyor->CurrentThroughput * 60, &FNumberFormattingOptions::DefaultNoGrouping())); // TODO use actual ticks/s
        Input->Set(Conveyor->SourceInv);
        Output->Set(Conveyor->TargetInv);
    }
}
