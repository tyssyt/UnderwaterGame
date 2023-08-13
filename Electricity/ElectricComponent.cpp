// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricComponent.h"

UElectricComponent::UElectricComponent() : State(PowerState::Initial) {
    PrimaryComponentTick.bCanEverTick = false;
}


PowerState UElectricComponent::GetState() const {
    return State;
}

void UElectricComponent::SetState(PowerState newState) { // TODO understand component lifecycle better and rework this
    check(newState != PowerState::Initial);
    if (State == newState)
        return;
    State = newState;

    // update symbol if necessary
    switch (State) {
    case PowerState::Disconnected:
    case PowerState::Unpowered:
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
    case PowerState::Powered:
        GetOwner()->SetActorTickEnabled(true); // propably want to do this another way because it can lead to weird behaviour if multiple systems are changing it
        if (DisabledSymbol) {
            GetOwner()->RemoveInstanceComponent(DisabledSymbol);
            DisabledSymbol->DestroyComponent();
            DisabledSymbol = nullptr;
        }
        break;
    }
}
