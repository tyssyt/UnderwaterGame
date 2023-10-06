// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricComponent.h"

#include "XD/CameraPawn.h"
#include "XD/GameInstanceX.h"
#include "PowerOverlay.h"
#include "Components/BillboardComponent.h"
#include "XD/Utils.h"

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
