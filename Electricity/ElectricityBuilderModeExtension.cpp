// Fill out your copyright notice in the Description page of Project Settings.

#include "ElectricityBuilderModeExtension.h"

#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Habitat.h"
#include "XD/Buildings/IndoorBuilding.h"
#include "XD/Electricity/ElectricComponent.h"
#include "XD/Electricity/ElectricityManager.h"

void UElectricityBuilderModeExtension::Init(ABuilding* preview, UConstructionUI* constructionUI) {
    const auto elec = preview->GetComponentByClass<UElectricComponent>();
    check(elec);

    Preview = preview;
    ConstructionUI = constructionUI;

    if (!preview->IsA<AIndoorBuilding>()) {
        constructionUI->TogglePower->SetVisibility(ESlateVisibility::Visible);
        WireComponent = UWireComponent::Create(preview, preview, preview);
        WireComponent->SetHiddenInGame(true);
    }

    const auto constructionMaterials = constructionUI->ConstructionMaterials;
    PowerResourceUI = CreateWidget<UResourceBalanceUI>(constructionMaterials, constructionMaterials->ResourceBalanceUIClass);
    PowerResourceUI->SetNeed(elec->Consumption, elec->GetElectricity());
    if (elec->Consumption > 0)
        constructionMaterials->AddExternalResource(PowerResourceUI);
}

void UElectricityBuilderModeExtension::Update() {
    const auto playerController = The::PlayerController(this);

    if (const auto indoor = Cast<AIndoorBuilding>(Preview)) {
        if (const auto habitat = indoor->Habitat) {
            if (const auto substation = habitat->GetComponentByClass<UElectricComponent>()->GetSubstation()) {
                PowerResourceUI->SetHave(substation->Network->GetTotalConstantProduction() - substation->Network->GetTotalConstantConsumption());
                return;
            }
        }
        PowerResourceUI->SetHave(0);
        return;
    }

    if (ConstructionUI->TogglePower->GetCheckedState() == ECheckBoxState::Unchecked) {
        Last = MakeTuple(FVector(), FRotator());
        ConnectedSubstation = nullptr;
        if (PowerUI)
            PowerUI->DestroyComponent();
        WireComponent->SetHiddenInGame(true);
        PowerResourceUI->SetHave(0);
        return;
    }

    if (MakeTuple(Preview->GetActorLocation(), Preview->GetActorRotation()) == Last)
        return;
    Last = MakeTuple(Preview->GetActorLocation(), Preview->GetActorRotation());

    ASubstation* nearestSubstation = The::ElectricityManager(Preview)->FindNearestSubstation(Last.Key);
    if (nearestSubstation != ConnectedSubstation) {
        ConnectedSubstation = nearestSubstation;
        if (PowerUI) {
            PowerUI->DestroyComponent();
        }

        if (nearestSubstation) {
            UPowerUI* ui = CreateWidget<UPowerUI>(playerController, playerController->BlueprintHolder->PowerUIClass);
            ui->Set(nearestSubstation->Network->GetTotalConstantProduction(), nearestSubstation->Network->GetTotalConstantConsumption());
            // TODO copied from Buildermode
            PowerUI = NewObject<UWidgetComponent>(nearestSubstation);
            PowerUI->RegisterComponent();
            PowerUI->AttachToComponent(nearestSubstation->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
            PowerUI->AddWorldOffset(FVector(0., 0. , 25.));
            PowerUI->SetWidgetSpace(EWidgetSpace::Screen);
            PowerUI->SetWidget(ui);
            PowerUI->SetDrawAtDesiredSize(true);
            PowerUI->SetPivot(FVector2D(0.5, 0.5));
            PowerUI->SetGenerateOverlapEvents(false);
            PowerUI->RecreatePhysicsState();
            nearestSubstation->AddInstanceComponent(PowerUI);
            
            WireComponent->SetHiddenInGame(false);
        } else {
            WireComponent->SetHiddenInGame(true);
        }
    }

    if (ConnectedSubstation) {
        // wire end is always relative to parent, so we need to update end pos
        WireComponent->SetEnd(ConnectedSubstation);
        PowerResourceUI->SetHave(ConnectedSubstation->Network->GetTotalConstantProduction() - ConnectedSubstation->Network->GetTotalConstantConsumption());
    } else {
        PowerResourceUI->SetHave(0);
    }
}

void UElectricityBuilderModeExtension::End(bool cancelled) {
    AutoConnectWires = ConstructionUI->TogglePower->GetCheckedState() == ECheckBoxState::Checked;
    if (WireComponent)
        WireComponent->DestroyComponent();
    if (PowerUI)
        PowerUI->DestroyComponent();
}
