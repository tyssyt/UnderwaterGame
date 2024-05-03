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

UDisconnected::UDisconnected() {   
    const static ConstructorHelpers::FObjectFinder<UTexture2D> SymbolFinder(TEXT("/Game/Assets/Images/Disconnected"));
    Symbol = SymbolFinder.Object;
    Type = !UObject::GetWorld() || The::Cheats(this)->AlwaysPowered ? EType::Visual : EType::TickDisabled;
}
UDeactivated::UDeactivated() {
    const static ConstructorHelpers::FObjectFinder<UTexture2D> SymbolFinder(TEXT("/Game/Assets/Images/Deactivated"));
    Symbol = SymbolFinder.Object;
    Type = !UObject::GetWorld() || The::Cheats(this)->AlwaysPowered ? EType::Visual : EType::TickDisabled;
}
UUnpowered::UUnpowered() {
    const static ConstructorHelpers::FObjectFinder<UTexture2D> SymbolFinder(TEXT("/Game/Assets/Images/Unpowered"));
    Symbol = SymbolFinder.Object;
    Type = !UObject::GetWorld() || The::Cheats(this)->AlwaysPowered ? EType::Visual : EType::TickDisabled;
}

UElectricComponent::UElectricComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

UElectricComponent* UElectricComponent::Init(int consumption) {
    State = PowerState::Initial;
    Consumption = consumption;
    return this;
}


void UElectricComponent::SetCondition(UCondition* condition) {
    const auto building = GetOwner<ABuilding>();
    if (Condition)
        building->RemoveCondition(Condition);
    Condition = condition;
    if (condition)
        building->AddCondition(condition);
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

void UElectricComponent::SetDisconnected() {
    check(GetType() != Type::IndoorBuilding);
    if (State == PowerState::Disconnected)
        return;
    
    const auto oldState = State;
    State = PowerState::Disconnected;
    SetCondition(NewObject<UDisconnected>(GetOwner()));
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
}
void UElectricComponent::SetConnected(ASubstation* substation) {
    check(GetType() != Type::IndoorBuilding);
    check(State == PowerState::Initial || State == PowerState::Disconnected);

    if (Consumption > 0) {
        State = PowerState::Unpowered; // set as unpowered, the next update of the network will power the building if enough power is in the network
        SetCondition(NewObject<UUnpowered>(GetOwner()));
    } else {
        State = PowerState::Powered; // producers are always powered
        SetCondition(nullptr);
    }
    Substation = substation;
    The::ElectricityManager(this)->Disconnected.Remove(this);
}
void UElectricComponent::SetDeactivated() {
    check(GetType() != Type::Habitat);
    check(State == PowerState::Powered || State == PowerState::Unpowered);
    check(Consumption > 0);

    State = PowerState::Deactivated;
    SetCondition(NewObject<UDeactivated>(GetOwner()));
}
void UElectricComponent::SetUnpowered() {
    if (State == PowerState::Unpowered)
        return;
    check(GetType() != Type::Habitat);
    check(State == PowerState::Powered || State == PowerState::Deactivated || (State == PowerState::Initial && GetType() == Type::IndoorBuilding));
    check(Consumption > 0);

    State = PowerState::Unpowered;
    SetCondition(NewObject<UUnpowered>(GetOwner()));
}
void UElectricComponent::SetPowered() {
    check(State == PowerState::Unpowered);
    check(GetType() != Type::IndoorBuilding || GetOwner<AIndoorBuilding>()->Habitat->GetComponentByClass<UElectricComponent>()->State == PowerState::Powered);

    State = PowerState::Powered;
    SetCondition(nullptr);
}

UBuilderModeExtension* UElectricComponent::CreateBuilderModeExtension() {
    return NewObject<UElectricityBuilderModeExtension>(GetOwner());
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
    components.Add(NewObject<UElectricComponentUI>(GetOwner())->Init(this));
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
