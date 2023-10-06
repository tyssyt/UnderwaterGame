// Fill out your copyright notice in the Description page of Project Settings.

#include "Conveyor.h"

#include <set>

#include "XD/Inventory/InventoryComponent.h"

#include <utility>

#include "ConstructionPlan.h"
#include "Splitter.h"


AConveyor* AConveyor::Create(UWorld* world, ABuilding* source, ABuilding* target, TArray<FVector> nodes, const UResource* resource) {
    AConveyor* conveyor = world->SpawnActor<AConveyor>();
    conveyor->Source = source;
    conveyor->Target = target;
    conveyor->Nodes = nodes;
    conveyor->Connect(resource);
    return conveyor;
}

AConveyor::AConveyor() {
    PrimaryActorTick.bCanEverTick = true;    
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));    
}

void AConveyor::BeginPlay() {
    Super::BeginPlay();
    //Connect(); // for conveyors placed in the editor
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

void AConveyor::Connect(const UResource* resource) {
    if (!Source || !Target || Source == Target) {
        UE_LOG(LogTemp, Error, TEXT("Invalid Connect on %s"), *this->GetName());
        return;
    }
    
    if (!resource) {
        resource = FindCommonResource(Source->GetComponentByClass<UInventoryComponent>(), Target->GetComponentByClass<UInventoryComponent>());
        if (!resource)
            return;
    }
    
    SourceInv = nullptr;
    TargetInv = nullptr;

    // TODO extract common code from hande Target & handle Source
    
    // handle Source    
    UInventoryComponent* sourceInventory = Source->FindComponentByClass<UInventoryComponent>();
    if (!sourceInventory)
        return;
    auto& outputs = sourceInventory->GetOutputs();
    for (auto& output : outputs) {
        if (output.Conveyor)
            continue;
        if (output.Resource == resource) {
            SourceInv = &output;
            break;
        }
        if (!output.Resource) {
            SourceInv = &output;
        }
    }
    check(SourceInv != nullptr);
    if (!SourceInv->Resource) // we found a untyped input, type it
        SourceInv->Resource = resource;
    SourceInv->Conveyor = this;
    if (ASplitter* splitter = Cast<ASplitter>(Source))
        splitter->Connections++;

    // handle Target
    UInventoryComponent* targetInventory = Target->FindComponentByClass<UInventoryComponent>();
    if (!targetInventory)
        return;
    auto& inputs = targetInventory->GetInputs();
    for (auto& input : inputs) {
        if (input.Conveyor)
            continue;
        if (input.Resource == resource) {
            TargetInv = &input;
            break;
        }
        if (!input.Resource) {
            TargetInv = &input;
        }
    }
    check(TargetInv != nullptr);
    if (!TargetInv->Resource) // we found a untyped input, type it
        TargetInv->Resource = resource;
    TargetInv->Conveyor = this;
    if (AMerger* merger = Cast<AMerger>(Target))
        merger->Connections++;

    // Visually Connect Source to Target via Nodes
    SetActorLocation((Source->GetActorLocation() + Target->GetActorLocation()) * .5f);
    
    FVector lastNode = Source->GetActorLocation();
    for (auto nextNode : Nodes) {
        MakeLink(lastNode, nextNode);
        MakeNode(nextNode);
        lastNode = nextNode;
    }
    MakeLink(lastNode, Target->GetActorLocation());
}

void AConveyor::MakeNode(FVector location) {
    UConveyorNode* node = NewObject<UConveyorNode>(this);
    node->RegisterComponent();
    node->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    node->SetWorldLocation(location);
    this->AddInstanceComponent(node);    
}

void AConveyor::MakeLink(FVector start, FVector end) {
    UConveyorLink* nextLink = NewObject<UConveyorLink>(this);
    nextLink->RegisterComponent();
    nextLink->AttachToComponent(GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    this->AddInstanceComponent(nextLink);
    nextLink->Connect(start, end);
}

const UResource* AConveyor::FindCommonResource(UInventoryComponent* source, UInventoryComponent* target) {
    if (!source || !target || source == target || source->GetOutputs().Num() == 0 || target->GetInputs().Num() == 0)
        return nullptr;

    auto& inputs = target->GetInputs();
    auto& outputs = source->GetOutputs();    
    
    // find matching in/outputs
    std::set<const UResource*> resources;
    bool hasUntypedInput = false;
    bool hasUntypedOutput = false;
    for (auto& output : outputs) {
        if (output.Conveyor)
            continue;
        if (!output.Resource) {
            hasUntypedOutput = true;
            continue;
        }
        for (auto& input : inputs) {
            if (input.Conveyor)
                continue;
            if (!input.Resource)
                hasUntypedInput = true;
            if (input.Resource == output.Resource)
                resources.insert(input.Resource);
        }
    }
    
    if (resources.size() == 0) { // no matches found, check for untyped in- or outputs
        if (hasUntypedInput) {

            resources.clear();
            for (auto& output : outputs) {
                if (!output.Conveyor)
                    resources.insert(output.Resource);
            }

            if (resources.size() == 1)
                return *resources.begin();
        }

        if (hasUntypedOutput) {

            resources.clear();
            for (auto& input : inputs) {
                if (!input.Conveyor)
                    resources.insert(input.Resource);
            }

            if (resources.size() == 1)
                return *resources.begin();
        }
        
        if (hasUntypedInput ^ hasUntypedOutput)
            UE_LOG(LogTemp, Error, TEXT("No Matches found, even though there is an Untyped Input/Output"));
        return nullptr;
    }
    if (resources.size() == 1) // exactly one match found, connect them
        return *resources.begin();

    // multiple matches found, show a dialog
    UE_LOG(LogTemp, Error, TEXT("Multiple Matches found"));
    return nullptr; // TODO show a dialog to select the input
    // TODO if the input/target is untyped, and there are multiple outputs/sources, also prompt
    // TODO think about if we want the vice versa of the above
    // TODO think about depot to depot connections... if neither of them is connected to something else in the chain...
}

TArray<Material> AConveyor::ComputeCosts(FVector start, FVector* end, TArray<FVector>& nodes, ESourceTargetType splitter, ESourceTargetType merger, UEncyclopedia* theEncyclopedia) {
    double linkDist = 0.;    
    FVector last = start;
    for (auto node : nodes) {
        linkDist += FVector::Distance(last, node);
        last = node;
    }
    if (end)
        linkDist += FVector::Distance(last, *end);

    return ComputeCosts(linkDist, nodes.Num(), splitter, merger, theEncyclopedia);
}

inline static constexpr double ConveyorLinkDistanceScale = 13.37; // TODO this needs to be explained somewhere in the encyclopedia
TArray<Material> AConveyor::ComputeCosts(double linkDist, int numNodes, ESourceTargetType splitter, ESourceTargetType merger, UEncyclopedia* theEncyclopedia) {
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

std::pair<AConveyor*, AConveyor*> AConveyor::SplitAt(UStaticMeshComponent* mesh, ABuilding* building) {
    TInlineComponentArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes);

    check(meshes.Contains(mesh));

    TArray<FVector> nodes1;
    TArray<FVector> nodes2;

    TArray<FVector>* cur = &nodes1;
    for (const auto m : meshes) {
        if (m == mesh) {
            cur = &nodes2;
            continue;
        }
        if (m->IsA<UConveyorNode>())
            cur->Push(m->GetComponentLocation());
    }

    // TODO this should propably go to some generic onDelete

    check(SourceInv->Conveyor == this);
    if (ASplitter* splitter = Cast<ASplitter>(Source))
        DisconnectFromSplitter(splitter);
    else
        SourceInv->Conveyor = nullptr;

    check(TargetInv->Conveyor == this);
    if (AMerger* merger = Cast<AMerger>(Target))
        DisconnectFromMerger(merger);
    else
        TargetInv->Conveyor = nullptr;
    
    this->Destroy();    
    return std::make_pair(Create(GetWorld(), Source, building, nodes1, SourceInv->Resource), Create(GetWorld(), building, Target, nodes2, SourceInv->Resource));
}

void AConveyor::DisconnectFromSplitter(ASplitter* splitter) const {
    TArray<FInventorySlot>* outputs = &splitter->Inventory->GetOutputs();
    FInventorySlot* slot = nullptr;
    for (auto& output : *outputs) {
        if (output.Conveyor == this) {
            slot = &output;
            break;
        }
    }
    check(slot != nullptr);
    FInventorySlot* last = &(*outputs)[splitter->Connections-1];
    if (slot != last) {
        // swap with last
        FInventorySlot tmp = *last;
        *last = *slot;
        *slot = tmp;
        // fix up pointer
        slot->Conveyor->SourceInv = slot; // NPE if the last slot was empty
    }
    // make sure no items are lost
    splitter->Inventory->GetInputs()[0].Current += last->Current;
    last->Current = 0;
    last->Conveyor = nullptr;
    splitter->Connections--;    
}

void AConveyor::DisconnectFromMerger(AMerger* merger) const {
    TArray<FInventorySlot>* inputs = &merger->Inventory->GetInputs();
    FInventorySlot* slot = nullptr;
    for (auto& input : *inputs) {
        if (input.Conveyor == this) {
            slot = &input;
            break;
        }
    }
    check(slot != nullptr);
    FInventorySlot* last = &(*inputs)[merger->Connections-1];
    if (slot != last) {
        // swap with last
        FInventorySlot tmp = *last;
        *last = *slot;
        *slot = tmp;
        // fix up pointer
        slot->Conveyor->TargetInv = slot;
    }
    // make sure no items are lost
    merger->Inventory->GetOutputs()[0].Current += last->Current;
    last->Current = 0;
    last->Conveyor = nullptr;
    merger->Connections--;    
}

UConveyorLink::UConveyorLink() {    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cube"));
    UStaticMeshComponent::SetStaticMesh(MeshFinder.Object);
}

void UConveyorLink::Connect(FVector start, FVector end) {     
    FVector startToEnd = end - start;

    // Nudge start end end a little bit towards each other, not changing the z component
    FVector nudge = FVector(startToEnd.X, startToEnd.Y, 0.);
    if (nudge.Normalize()) {
        start = start + 13 * nudge;
        end = end - 13 * nudge;
        startToEnd = end - start;
    }
    
    SetWorldLocation((start + end) * .5f);
    SetWorldRotation(startToEnd.Rotation());
    SetWorldScale3D(FVector(startToEnd.Size() / 100.f, .1f, 0.1f));
}

UConveyorNode::UConveyorNode() {    
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    UStaticMeshComponent::SetStaticMesh(MeshFinder.Object);
    SetWorldScale3D(FVector(.3, .3, .1));
}

void UConveyorUI::Tick() {
    if (Conveyor) {
        CurrentThroughput->SetText(FText::AsNumber(Conveyor->CurrentThroughput * 60)); // TODO use actual ticks/s
        Input->Set(Conveyor->SourceInv);
        Output->Set(Conveyor->TargetInv);
    }
}
