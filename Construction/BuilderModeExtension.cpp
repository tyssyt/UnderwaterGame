// Fill out your copyright notice in the Description page of Project Settings.


#include "BuilderModeExtension.h"

#include "XD/GameInstanceX.h"
#include "XD/PlayerControllerX.h"

void UElectricityBuilderModeExtension::Init(ABuilding* preview) {
    UElectricComponent* elec = preview->GetComponentByClass<UElectricComponent>();
    check(elec);
    
    Preview = preview;
    WireComponent = UWireComponent::Create(preview, preview, preview);
    WireComponent->SetHiddenInGame(true);

    const auto constructionUI = GetWorld()->GetFirstPlayerController<APlayerControllerX>()->BlueprintHolder->ConstructionUI;
    constructionUI->TogglePower->SetVisibility(ESlateVisibility::Visible);

    PowerResourceUI = CreateWidget<UResourceBalanceUI>(constructionUI, constructionUI->ResourceBalanceUIClass);
    PowerResourceUI->SetNeed(elec->Consumption, GetWorld()->GetGameInstance<UGameInstanceX>()->TheResourceBook->Electricity);
    constructionUI->AddExternalResource(PowerResourceUI);
}

void UElectricityBuilderModeExtension::Update() {
    APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();

    if (playerController->BlueprintHolder->ConstructionUI->TogglePower->GetCheckedState() == ECheckBoxState::Unchecked) {
        Last = std::make_pair(FVector(), FRotator());
        ConnectedSubstation = nullptr;
        if (PowerUI)
            PowerUI->DestroyComponent();
        WireComponent->SetHiddenInGame(true);
        PowerResourceUI->SetHave(0);
        return;
    }
    
    if (std::make_pair(Preview->GetActorLocation(), Preview->GetActorRotation()) == Last)
        return;
    Last = std::make_pair(Preview->GetActorLocation(), Preview->GetActorRotation());

    ASubstation* nearestSubstation = Preview->GetGameInstance()->TheElectricityManager->FindNearestSubstation(Last.first);
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

void UElectricityBuilderModeExtension::End() {
    WireComponent->DestroyComponent();
    if (PowerUI)
        PowerUI->DestroyComponent();
}

void USubstationBuilderModeExtension::Init(ABuilding* preview) {
    check(preview->IsA(ASubstation::StaticClass()));
    Preview = Cast<ASubstation>(preview);

    const auto constructionUI = GetWorld()->GetFirstPlayerController<APlayerControllerX>()->BlueprintHolder->ConstructionUI;
    constructionUI->TogglePower->SetVisibility(ESlateVisibility::Visible);
}

void USubstationBuilderModeExtension::Update() {
    const APlayerControllerX* playerController = GetWorld()->GetFirstPlayerController<APlayerControllerX>();

    if (playerController->BlueprintHolder->ConstructionUI->TogglePower->GetCheckedState() == ECheckBoxState::Unchecked) {
        Last = std::make_pair(FVector(), FRotator());
        
        for (UWireComponent* wire : Wires)
            wire->DestroyComponent();
        Wires.Empty();
        return;
    }
    
    if (std::make_pair(Preview->GetActorLocation(), Preview->GetActorRotation()) == Last)
        return;
    Last = std::make_pair(Preview->GetActorLocation(), Preview->GetActorRotation());

    for (UWireComponent* wire : Wires)
        wire->DestroyComponent();
    Wires.Empty();

    const auto nearby = Preview->FindNearby();
    
    TArray<ElectricityNetwork*> connectedNetworks;
    for (ASubstation* nearbySubstation : nearby.first) {
        Wires.Add(UWireComponent::Create(Preview, Preview, nearbySubstation));
        if (!connectedNetworks.Contains(nearbySubstation->Network))
            connectedNetworks.Add(nearbySubstation->Network);
    }
    
    for (const UElectricComponent* nearbyElec : nearby.second) {
        const FVector elecLocation = nearbyElec->GetOwner()->GetActorLocation();
        if (nearbyElec->Substation) {
            // check if we would be on the same network and closer then the substation currently used
            if (FVector::Distance(Last.first, elecLocation) < FVector::Distance(nearbyElec->Substation->GetActorLocation(), elecLocation)
                    && connectedNetworks.Contains(nearbyElec->Substation->Network)) {
                Wires.Add(UWireComponent::Create(Preview, Preview, nearbyElec->GetOwner<ABuilding>()));
            }
        } else {     
            Wires.Add(UWireComponent::Create(Preview, Preview, nearbyElec->GetOwner<ABuilding>()));       
        }
    }
}

void USubstationBuilderModeExtension::End() {
    for (UWireComponent* wire : Wires)
        wire->DestroyComponent();
}

void UIndoorElectricityBuilderModeExtension::Init(ABuilding* preview) {
    checkNoEntry(); // TODO this is wip, I need a special kind of electric component for indoors, because it should connect to the habitat
    check(preview->IsA(AIndoorBuilding::StaticClass()));
    Preview = Cast<AIndoorBuilding>(preview);

    UElectricComponent* elec = preview->GetComponentByClass<UElectricComponent>();
    check(elec);
    
    const auto constructionUI = GetWorld()->GetFirstPlayerController<APlayerControllerX>()->BlueprintHolder->ConstructionUI;
    PowerResourceUI = CreateWidget<UResourceBalanceUI>(constructionUI, constructionUI->ResourceBalanceUIClass);
    PowerResourceUI->SetNeed(elec->Consumption, GetWorld()->GetGameInstance<UGameInstanceX>()->TheResourceBook->Electricity);
    constructionUI->AddExternalResource(PowerResourceUI);
}

void UIndoorElectricityBuilderModeExtension::Update() {
    if (Preview->Habitat) {
        if (const ASubstation* substation = Preview->Habitat->Electricity->Substation)
            PowerResourceUI->SetHave(substation->Network->GetTotalConstantProduction() - substation->Network->GetTotalConstantConsumption());
        else 
            PowerResourceUI->SetHave(0); // maybe show the complete power deficit if the habitat
    } else
        PowerResourceUI->SetHave(0);
}

void UIndoorElectricityBuilderModeExtension::End() {
}
