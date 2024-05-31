// Fill out your copyright notice in the Description page of Project Settings.

#include "Substation.h"

#include "IndoorBuilding.h"
#include "The.h"
#include "Components/VerticalBoxSlot.h"
#include "XD/BlueprintHolder.h"

void ASubstation::Add(UElectricComponent* building) {
    switch (building->GetType()) {
    case UElectricComponent::Type::OutdoorBuilding:
        ConnectedBuildings.Add(building);
        break;
    case UElectricComponent::Type::Habitat:
        ConnectedHabitats.Add(building);
        break;
    case UElectricComponent::Type::IndoorBuilding:;
    }    
}
void ASubstation::Remove(UElectricComponent* building) {
    switch (building->GetType()) {
    case UElectricComponent::Type::OutdoorBuilding:
        ConnectedBuildings.Remove(building);
        break;
    case UElectricComponent::Type::Habitat:
        ConnectedHabitats.Remove(building);
        break;
    case UElectricComponent::Type::IndoorBuilding:;
    }
}

ASubstation::ASubstation() {
    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/Substation"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);
}

void ASubstation::Connect(UElectricComponent* building, bool recomputeStats) {
    check(building->GetType() != UElectricComponent::Type::IndoorBuilding);
    
    if (const auto oldSubstation = building->GetSubstation()) {
        if (oldSubstation->Network == this->Network) {
            oldSubstation->Remove(building);
            this->Add(building);
            return;
        }
        oldSubstation->Disconnect(building);
    }

    check(building->GetState() == PowerState::Disconnected || building->GetState() == PowerState::Initial);
    check(building->GetSubstation() == nullptr);

    Add(building);
    building->SetConnected(this);
    if (recomputeStats)
        Network->RecomputeStats();
}
void ASubstation::Disconnect(UElectricComponent* building) {
    check(building->GetType() != UElectricComponent::Type::IndoorBuilding);
    check(building->GetState() == PowerState::Powered || building->GetState() == PowerState::Unpowered || building->GetState() == PowerState::Deactivated);
    check(building->GetSubstation() == this);

    Remove(building);
    building->SetDisconnected();
    Network->RecomputeStats();
}

void ASubstation::DisconnectFromNetwork() {
    check(Network->Substations.Num() > 1);
    ElectricityNetwork* old = Network;
    Network = new ElectricityNetwork(this);
    old->Substations.Remove(this);
    old->CheckForNetworkSplit();
    Network->RecomputeStats();
}


bool isAutoConnect(UBuilderModeExtension* extension) {
    if (const auto elec = Cast<USubstationBuilderModeExtension>(extension))
        return elec->AutoConnectWires;
    return false;
}
void ASubstation::OnConstructionComplete(UBuilderModeExtensions* extensions) {
    Super::OnConstructionComplete(extensions);

    if (!Cast<USubstationBuilderModeExtension>(extensions->BuildingExtension)->AutoConnectWires) {
        Network = new ElectricityNetwork(this);
        Network->RecomputeStats();
        return;
    }

    const auto nearby = FindNearby();
    if (!nearby.Key.IsEmpty()) {
        // join & merge nearby networks
        Network = nearby.Key[0]->Network;
        Network->Substations.Add(this);
        for (const ASubstation* nearbySubstation : nearby.Key) {
            Network->MergeNetworkNoRecompute(nearbySubstation->Network);
        }
    } else
        Network = new ElectricityNetwork(this);

    for (const auto nearbyElec : nearby.Value) {
        if (const auto otherSubstation = nearbyElec->GetSubstation()) {
            // check if we are on the same network and closer then the substation currently used
            const FVector elecLoc = nearbyElec->GetOwner()->GetActorLocation();
            if (this->Network == otherSubstation->Network
                && FVector::Distance(GetActorLocation(), elecLoc) < FVector::Distance(otherSubstation->GetActorLocation(), elecLoc)
            ) {
                otherSubstation->Remove(nearbyElec);
                this->Add(nearbyElec);
            }
        } else {
            // Always connect unconnected components
            check(nearbyElec->GetState() == PowerState::Disconnected);
            Add(nearbyElec);
            nearbyElec->SetConnected(this);
        }
    }

    Network->RecomputeStats();
}

void ASubstation::OnDismantleStart() {
    Super::OnDismantleStart();

    if (Network->Substations.Num() == 1) {
        while (!ConnectedBuildings.IsEmpty())
            Disconnect(ConnectedBuildings.Last());
        while (!ConnectedHabitats.IsEmpty())
            Disconnect(ConnectedHabitats.Last());
        delete Network;
        Network = nullptr;
        return;
    }

    Network->Substations.Remove(this);
    while (!ConnectedBuildings.IsEmpty()) {
        const auto building = ConnectedBuildings.Pop();
        if (const auto substation = Network->FindNearestSubstation(building->GetOwner()->GetActorLocation()))
            substation->Connect(building, false);
        else
            Disconnect(building);
    }
    while (!ConnectedHabitats.IsEmpty()) {
        const auto building = ConnectedHabitats.Pop();
        if (const auto substation = Network->FindNearestSubstation(building->GetOwner()->GetActorLocation()))
            substation->Connect(building, false);
        else
            Disconnect(building);
    }

    Network->CheckForNetworkSplit();
    Network = nullptr;
}

UBuilderModeExtensions* ASubstation::CreateBuilderModeExtension() {
    const auto extensions = Super::CreateBuilderModeExtension();
    extensions->BuildingExtension = NewObject<USubstationBuilderModeExtension>(this);
    return extensions;
}

TPair<TArray<ASubstation*>, TArray<UElectricComponent*>> ASubstation::FindNearby() const {
    const static FName NAME_QUERY_PARAMS = FName(TEXT(""));
    const FCollisionQueryParams queryParams(NAME_QUERY_PARAMS, false, this);
    const FCollisionObjectQueryParams objectQueryParams = FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects);
    // TODO make a custom collision channel with substations and maybe electricComponents

    TArray<FOverlapResult> overlaps;
    GetWorld()->OverlapMultiByObjectType(
        overlaps,
        GetActorLocation(),
        FQuat::Identity,
        objectQueryParams,
        FCollisionShape::MakeSphere(ElectricityNetwork::MAX_WIRE_DISTANCE),
        queryParams
    );

    TArray<ASubstation*> nearbySubstations;
    TArray<UElectricComponent*> nearbyElecs;
    for (const FOverlapResult& overlap : overlaps) {
        const auto nearbySubstation = Cast<ASubstation>(overlap.GetActor());
        if (nearbySubstation && !nearbySubstation->IsNonInteractable() && FVector::Distance(
            GetActorLocation(),
            nearbySubstation->GetActorLocation()
        ) < ElectricityNetwork::MAX_WIRE_DISTANCE) {
            nearbySubstations.Add(nearbySubstation);
        }

        if (overlap.GetActor()->IsA<AIndoorBuilding>())
            continue;
        const auto nearbyElec = overlap.GetActor()->FindComponentByClass<UElectricComponent>();
        if (nearbyElec && nearbyElec->GetState() != PowerState::Initial && FVector::Distance(
            GetActorLocation(),
            nearbyElec->GetOwner()->GetActorLocation()
        ) < ElectricityNetwork::MAX_WIRE_DISTANCE) {
            nearbyElecs.Add(nearbyElec);
        }
    }

    return MakeTuple(MoveTemp(nearbySubstations), MoveTemp(nearbyElecs));
}

void ASubstation::InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {    
    components.Add(NewObject<USubstationUIComponent>(this)->Init(this));
    Super::InitSelectedUI(components);
}

USubstationUIComponent* USubstationUIComponent::Init(ASubstation* substation) {
    Substation = substation;
    return this;
}

void USubstationUIComponent::CreateUI(UBuildingSelectedUI* selectedUI) {
    UI = CreateWidget<USubstationUI>(selectedUI->WidgetTree, The::BPHolder(this)->SubstationUIClass);
    const auto slot = selectedUI->Content->AddChildToVerticalBox(UI);
    slot->SetVerticalAlignment(VAlign_Center);
    slot->SetHorizontalAlignment(HAlign_Fill);
}

void USubstationUIComponent::Tick(UBuildingSelectedUI* selectedUI) {
    if (const auto network = Substation->Network) {
        const int production = network->GetTotalConstantProduction();
        const int consumption = network->GetTotalConstantConsumption();
        UI->PowerUI->Set(production, consumption);
        UI->FillLevel->SetPercent(static_cast<float>(consumption) / production);
    } else {
        UI->PowerUI->Set(0, 0);
        UI->FillLevel->SetPercent(0.);
    }
}

void USubstationBuilderModeExtension::Update() {
    if (ConstructionUI->TogglePower->GetCheckedState() == ECheckBoxState::Unchecked) {
        Last = MakeTuple(FVector(), FRotator());
        
        for (UWireComponent* wire : Wires)
            wire->DestroyComponent();
        Wires.Empty();
        return;
    }
    
    if (MakeTuple(Preview->GetActorLocation(), Preview->GetActorRotation()) == Last)
        return;
    Last = MakeTuple(Preview->GetActorLocation(), Preview->GetActorRotation());

    for (UWireComponent* wire : Wires)
        wire->DestroyComponent();
    Wires.Empty();

    const auto nearby = Preview->FindNearby();
    
    TArray<ElectricityNetwork*> connectedNetworks;
    for (ASubstation* nearbySubstation : nearby.Key) {
        Wires.Add(UWireComponent::Create(Preview, Preview, nearbySubstation));
        if (!connectedNetworks.Contains(nearbySubstation->Network))
            connectedNetworks.Add(nearbySubstation->Network);
    }
    
    for (const UElectricComponent* nearbyElec : nearby.Value) {
        const FVector elecLocation = nearbyElec->GetOwner()->GetActorLocation();
        if (const auto substation = nearbyElec->GetSubstation()) {
            // check if we would be on the same network and closer then the substation currently used
            if (FVector::Distance(Last.Key, elecLocation) < FVector::Distance(substation->GetActorLocation(), elecLocation)
                    && connectedNetworks.Contains(substation->Network)) {
                Wires.Add(UWireComponent::Create(Preview, Preview, nearbyElec->GetOwner<ABuilding>()));
                    }
        } else {     
            Wires.Add(UWireComponent::Create(Preview, Preview, nearbyElec->GetOwner<ABuilding>()));       
        }
    }
}

void USubstationBuilderModeExtension::End(bool cancelled) {
    AutoConnectWires = ConstructionUI->TogglePower->GetCheckedState() == ECheckBoxState::Checked;

    for (const auto wire : Wires)
        wire->DestroyComponent();
}
