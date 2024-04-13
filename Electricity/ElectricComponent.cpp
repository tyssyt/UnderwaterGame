// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricComponent.h"

#include "ElectricityBuilderModeExtension.h"
#include "ElectricityManager.h"
#include "PowerOverlay.h"
#include "The.h"
#include "UI.h"
#include "XD/BlueprintHolder.h"
#include "XD/CameraPawn.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"
#include "XD/Buildings/Substation.h"
#include "XD/Construction/BuilderModeExtension.h"

UElectricComponent::UElectricComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

UElectricComponent* UElectricComponent::Init(int consumption) {
    State = PowerState::Initial;
    Consumption = consumption;
    return this;
}

UElectricComponent::Type UElectricComponent::GetType() const {
    const auto owner = GetOwner();
    if (owner->IsA<AIndoorBuilding>())
        return Type::IndoorBuilding;
    if (owner->IsA<AHabitat>())
        return Type::Habitat;
    return Type::OutdoorBuilding;
}

ASubstation* UElectricComponent::GetSubstation() const {
    switch (GetType()) {
    case Type::IndoorBuilding:
        return this->GetOwner<AIndoorBuilding>()->Habitat->GetComponentByClass<UElectricComponent>()->Substation;
    case Type::OutdoorBuilding: case Type::Habitat:
        return Substation;
    }
    checkNoEntry();
    return nullptr;
}

UResource* UElectricComponent::GetElectricity() const {
    return ComponentInfo->Needs[0].Resource;
}

// TODO have a management thing on building that can show symbols and disable/enable ticking that does roughly as below
// void UElectricComponent::SetState(const PowerState newState) {
//     // update symbol if necessary
//     switch (State) {
//     case PowerState::Disconnected:
//     case PowerState::Deactivated:
//     case PowerState::Unpowered: {
//         if (Cheats::ALWAYS_POWERED)
//             break;
//         GetOwner()->SetActorTickEnabled(false); // propably want to do this another way because it can lead to weird behaviour if multiple systems are changing it
//         if (!DisabledSymbol) {
//             DisabledSymbol = NewObject<UBillboardComponent>(GetOwner(), TEXT("PowerDisabledSymbol"));
//             DisabledSymbol->RegisterComponent();
//             DisabledSymbol->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
//
//             DisabledSymbol->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Unpowered"))); // TODO probably want to load this in another way?
//             DisabledSymbol->SetRelativeLocation(FVector(.0f, .0f, 60.f));
//             DisabledSymbol->SetHiddenInGame(false);
//             DisabledSymbol->SetVisibility(true);
//             GetOwner()->AddInstanceComponent(DisabledSymbol);
//         }
//         break;
//     }
//     case PowerState::Powered: {
//         GetOwner()->SetActorTickEnabled(true); // propably want to do this another way because it can lead to weird behaviour if multiple systems are changing it
//         if (DisabledSymbol) {
//             GetOwner()->RemoveInstanceComponent(DisabledSymbol);
//             DisabledSymbol->DestroyComponent();
//             DisabledSymbol = nullptr;
//         }
//         break;
//     }

void UElectricComponent::SetDisconnected() {
    check(GetType() != Type::IndoorBuilding);
    if (State == PowerState::Disconnected)
        return;
    
    const auto oldState = State;
    State = PowerState::Disconnected;
    Substation = nullptr;

    The::ElectricityManager(this)->Disconnected.Add(this);
    // Weird special case, when going from Initial to Disconnected we don't trigger an UI Update, so we need to catch and handle it here
    if (oldState == PowerState::Initial)     
        The::CameraPawn(this)->PowerOverlay->AddDisconnected(this);

    // when disconnecting a Habitat, we need to set all consumers in the Habitat to Unpowered
    if (GetType() == Type::Habitat)
        for (const auto building : GetOwner<AHabitat>()->Buildings)
            if (const auto elec = building->GetComponentByClass<UElectricComponent>())
                elec->SetUnpowered();

    // TODO update symbol
}
void UElectricComponent::SetConnected(ASubstation* substation) {
    check(GetType() != Type::IndoorBuilding);
    check(State == PowerState::Initial || State == PowerState::Disconnected);

    if (Consumption > 0)
        State = PowerState::Unpowered; // set as unpowered, the next update of the network will power the building if enough power is in the network
    else
        State = PowerState::Powered; // producers are always powered
    Substation = substation;
    The::ElectricityManager(this)->Disconnected.Remove(this);
    // TODO update symbol
}
void UElectricComponent::SetDeactivated() {
    check(GetType() != Type::Habitat);
    check(State == PowerState::Powered || State == PowerState::Unpowered);
    check(Consumption > 0);

    State = PowerState::Deactivated;
    // TODO update symbol
}
void UElectricComponent::SetUnpowered() {
    if (State == PowerState::Unpowered)
        return;
    check(GetType() != Type::Habitat);
    check(State == PowerState::Powered || State == PowerState::Deactivated || (State == PowerState::Initial && GetType() == Type::IndoorBuilding));
    check(Consumption > 0);

    State = PowerState::Unpowered;
    // TODO update symbol
}
void UElectricComponent::SetPowered() {
    check(State == PowerState::Unpowered);
    check(GetType() != Type::IndoorBuilding || GetOwner<AIndoorBuilding>()->Habitat->GetComponentByClass<UElectricComponent>()->State == PowerState::Powered);

    State = PowerState::Powered;
    // TODO update symbol
}

UBuilderModeExtension* UElectricComponent::CreateBuilderModeExtension() {
    return NewObject<UElectricityBuilderModeExtension>(this);
}

void UElectricComponent::OnConstructionComplete(UBuilderModeExtension* extension) {
    switch (GetType()) {
    case Type::OutdoorBuilding: case Type::Habitat: {
        if (!extension || Cast<UElectricityBuilderModeExtension>(extension)->AutoConnectWires)
            if (const auto substation = The::ElectricityManager(this)->FindNearestSubstation(GetOwner()->GetActorLocation()))
                substation->Connect(this);
            else
                SetDisconnected();
        else
            SetDisconnected();
        break;
    }
    case Type::IndoorBuilding: {
        SetUnpowered();
        if (const auto substation = GetSubstation())
            substation->Network->RecomputeStats();
        break;
    }}
}

void UElectricComponent::AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    components.Add(NewObject<UElectricComponentUI>(this)->Init(this));
}

UElectricComponentUI* UElectricComponentUI::Init(UElectricComponent* electricComponent) {
    ElectricComponent = electricComponent;
    return this;
}

void UElectricComponentUI::CreateUI(UBuildingSelectedUI* selectedUI) {
    if (ElectricComponent->Consumption > 0) {
        UI = CreateWidget<UResourceBalanceUI>(
            The::PlayerController(this),
            The::BPHolder(this)->ResourceBalanceUIClass);
        UI->SetNeed(ElectricComponent->Consumption, ElectricComponent->GetElectricity());
        UI->SetHave(ElectricComponent->State == PowerState::Powered ? ElectricComponent->Consumption : 0);
        selectedUI->Top->AddChildToWrapBox(UX::Sized(selectedUI->WidgetTree, UI, 60.f, 60.f));
    } else {
        selectedUI->Top->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(
            The::PlayerController(this),
            The::BPHolder(this)->ResourceAmountUIClass
        )->Init(-ElectricComponent->Consumption, ElectricComponent->GetElectricity()));
    }
}

void UElectricComponentUI::Tick(UBuildingSelectedUI* selectedUI) {
    if (ElectricComponent->Consumption > 0)
        UI->SetHave(ElectricComponent->State == PowerState::Powered ? ElectricComponent->Consumption : 0);
}
