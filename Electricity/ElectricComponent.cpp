// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricComponent.h"

#include "ElectricityManager.h"
#include "PowerOverlay.h"
#include "The.h"
#include "UI.h"
#include "Components/BillboardComponent.h"
#include "XD/BlueprintHolder.h"
#include "XD/CameraPawn.h"
#include "XD/Cheats.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Buildings/Substation.h"
#include "XD/Construction/BuilderModeExtension.h"

UElectricComponent::UElectricComponent() : State(PowerState::Initial) {
    PrimaryComponentTick.bCanEverTick = false;
}

PowerState UElectricComponent::GetState() const {
    return State;
}

void UElectricComponent::SetState(const PowerState newState) { // TODO understand component lifecycle better and rework this
    check(newState != PowerState::Initial);
    if (State == newState)
        return;

    // TODO there is too much logic inside this setter, extract that

    if (State == PowerState::Disconnected) {
        The::ElectricityManager(this)->Disconnected.Remove(this);
    } else if (newState == PowerState::Disconnected) {
        The::ElectricityManager(this)->Disconnected.Add(this);

        // Weird special case, when going from Initial to Disconnected we don't trigger an UI Update, so we need to catch and handle it here
        if (State == PowerState::Initial) {
            State = newState;
            The::CameraPawn(this)->PowerOverlay->AddDisconnected(this);
        }
    }
    
    State = newState;

    // update symbol if necessary
    switch (State) {
    case PowerState::Disconnected:
    case PowerState::Deactivated:
    case PowerState::Unpowered: {
        if (Cheats::ALWAYS_POWERED)
            break;
        GetOwner()->SetActorTickEnabled(false); // propably want to do this another way because it can lead to weird behaviour if multiple systems are changing it
        if (!DisabledSymbol) {
            DisabledSymbol = NewObject<UBillboardComponent>(GetOwner(), TEXT("PowerDisabledSymbol"));
            DisabledSymbol->RegisterComponent();
            DisabledSymbol->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

            DisabledSymbol->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Resources/Unpowered"))); // TODO probably want to load this in another way?
            DisabledSymbol->SetRelativeLocation(FVector(.0f, .0f, 60.f));
            DisabledSymbol->SetHiddenInGame(false);
            DisabledSymbol->SetVisibility(true);
            GetOwner()->AddInstanceComponent(DisabledSymbol);
        }
        break;
    }
    case PowerState::Powered: {
        GetOwner()->SetActorTickEnabled(true); // propably want to do this another way because it can lead to weird behaviour if multiple systems are changing it
        if (DisabledSymbol) {
            GetOwner()->RemoveInstanceComponent(DisabledSymbol);
            DisabledSymbol->DestroyComponent();
            DisabledSymbol = nullptr;
        }
        break;
    }
    default: checkNoEntry();
    }
}

UResource* UElectricComponent::GetElectricity() const {
    return ComponentInfo->Needs[0].Resource;
}

TSubclassOf<UBuilderModeExtension> UElectricComponent::GetBuilderModeExtension() const {
    return UElectricityBuilderModeExtension::StaticClass();
}

void UElectricComponent::OnConstructionComplete(UConstructionOptions* options) {
    bool connected = false;

    const auto electricityOptions = options->Get<UElectricityConstructionOption>(UElectricityBuilderModeExtension::StaticClass());
    if (!electricityOptions || electricityOptions->AutoConnectWires) {
        if (ASubstation* substation = The::ElectricityManager(this)->FindNearestSubstation(GetOwner()->GetActorLocation())) {
            substation->Connect(this);
            connected = true;
        }
    }

    if (!connected)
        SetState(PowerState::Disconnected);
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
