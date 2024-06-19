// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerOverlay.h"

#include "ElectricityManager.h"
#include "ScalingWidgetComponent.h"
#include "The.h"
#include "XD/BlueprintHolder.h"
#include "XD/CollisionProfiles.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Habitat.h"

UPowerOverlay::UPowerOverlay() : Active(false) {
    HighlightPowered = NewObject<UHighlighted>()->SetColor(UHighlighted::Green);
    HighlightUnpowered = NewObject<UHighlighted>()->SetColor(UHighlighted::Red);
    HighlightDeactivated = NewObject<UHighlighted>()->SetColor(UHighlighted::Gray);
    HighlightUnderCursor = NewObject<UHighlighted>()->SetColor(UHighlighted::Yellow);
}

void UPowerOverlay::Tick(float DeltaTime) {
    // TODO if this never explodes, we can remove it at some point
    if (LastFrameNumberWeTicked == GFrameCounter) {
        checkNoEntry();
        return;
    }
    LastFrameNumberWeTicked = GFrameCounter;

    if (!Active)
        return;
    
    const EPowerOverlayMode mode = The::BPHolder(this)->PowerOverlayUI->GetMode();
    if (mode != ModeHighlight.Mode) {
        ResetModeHighlight(true);
        ModeHighlight.Mode = mode;

        if (mode == EPowerOverlayMode::Disconnect) {
            // set collision of wires
            TInlineComponentArray<UWireComponent*> wires;
            ComponentHolder->GetComponents<UWireComponent>(wires, true);
            for (UWireComponent* wire : wires)
                wire->SetCollisionProfileName(CollisionProfiles::UI);
        }
    }

    switch (mode) {
    case EPowerOverlayMode::None:
        return;
    case EPowerOverlayMode::TogglePower:
        TickTogglePower();
        break;
    case EPowerOverlayMode::Connect:
        TickConnect();
        break;
    case EPowerOverlayMode::Disconnect:
        TickDisconnect();
        break;
    default:
        checkNoEntry();
    }
}

void UPowerOverlay::TickTogglePower() {    
    const UElectricComponent* elec = CheckElec(The::PlayerController(this)->GetUnderCursor<ABuilding>());

    // can't toggle non interactable Buildings
    if (elec && elec->GetOwner<ABuilding>()->IsNonInteractable())
        elec = nullptr;

    // can't toggle disconnected Buildings
    if (elec && elec->GetState() == PowerState::Disconnected)
        elec = nullptr;

    // can't toggle power producers
    if (elec && elec->Consumption <= 0)
        elec = nullptr;

    // can't toggle power to habitats
    if (elec && elec->GetType() == UElectricComponent::Type::Habitat)
        elec = nullptr;

    // can't toggle buildings in disconnected habitats
    if (elec && elec->GetType() == UElectricComponent::Type::IndoorBuilding
        && elec->GetOwner<AIndoorBuilding>()->Habitat->GetComponentByClass<UElectricComponent>()->GetState() == PowerState::Disconnected)
        elec = nullptr;

    ABuilding* underCursor = elec ? elec->GetOwner<ABuilding>() : nullptr;
    if (underCursor == ModeHighlight.Current) {
        return;
    }

    ResetTogglePower();

    // add new Highlight
    if (underCursor) {
        switch (elec->GetState()) {
        case PowerState::Powered:
        case PowerState::Unpowered:
            underCursor->AddCondition(HighlightDeactivated);
            break;
        case PowerState::Deactivated:
            underCursor->AddCondition(HighlightPowered);
            break;
        case PowerState::Disconnected:
        case PowerState::Initial:
            checkNoEntry();
            break;
        } 
    }

    ModeHighlight.Current = underCursor;
}

void UPowerOverlay::TickConnect() {
    ABuilding* underCursor = The::PlayerController(this)->GetUnderCursor<ABuilding>();
    if (underCursor && underCursor->IsNonInteractable())
        underCursor = nullptr;

    if (const auto elec = CheckElec(underCursor)) {
        // can't connect indoor
        if (elec->GetType() == UElectricComponent::Type::IndoorBuilding)
            underCursor = nullptr;
    }

    if (!ModeHighlight.Source) {
        if (!CheckElec(underCursor) && !Cast<ASubstation>(underCursor))
            underCursor = nullptr;

        if (underCursor == ModeHighlight.Current)
            return;

        ResetConnect(false);

        if (underCursor)
            underCursor->AddCondition(HighlightUnderCursor);

        ModeHighlight.Current = underCursor;
    } else {
        UWireComponent* oldLink = nullptr;
        if (underCursor && !CanBeConnected(ModeHighlight.Source, underCursor, oldLink))
            underCursor = nullptr;
        
        if (underCursor == ModeHighlight.Current)
            return;

        ResetConnect(false);

        if (underCursor) {
            underCursor->AddCondition(HighlightUnderCursor);
            ModeHighlight.Current = underCursor;
            ModeHighlight.Wire = UWireComponent::Create(ComponentHolder, ModeHighlight.Source, underCursor);
            ModeHighlight.Wire->SetMaterial(0, HighlightPowered->GetMaterial());
            if (oldLink) {
                oldLink->SetMaterial(0, HighlightUnpowered->GetMaterial());
                ModeHighlight.Wires.Add(oldLink);
            }
        }

        ModeHighlight.Current = underCursor;
    }
}

void UPowerOverlay::TickDisconnect() {
    FHitResult hitResult;
    The::PlayerController(this)->GetHitResultUnderCursor(ECC_Visibility, true, hitResult);
    UWireComponent* wire = Cast<UWireComponent>(hitResult.GetComponent());

    ASubstation* substation = nullptr;
    if (wire && wire->GetStart()->IsA(ASubstation::StaticClass()) && wire->GetEnd()->IsA(ASubstation::StaticClass())) {
        if (FVector::Distance(hitResult.ImpactPoint, wire->GetStart()->GetActorLocation()) < FVector::Distance(hitResult.ImpactPoint, wire->GetEnd()->GetActorLocation()))
            substation = Cast<ASubstation>(wire->GetStart());
        else
            substation = Cast<ASubstation>(wire->GetEnd());
    }
    
    if (ModeHighlight.Wire == wire && ModeHighlight.Current == substation)
        return;

    ResetDisconnect(false);

    if (wire) {
        if (substation) {
            TInlineComponentArray<UWireComponent*> wires;
            ComponentHolder->GetComponents<UWireComponent>(wires, true);
            for (UWireComponent* w : wires) {
                if ((w->GetStart() == substation && w->GetEnd()->IsA(ASubstation::StaticClass())) ||
                    (w->GetEnd() == substation && w->GetStart()->IsA(ASubstation::StaticClass()))) {
                    w->SetMaterial(0, HighlightUnpowered->GetMaterial());
                    ModeHighlight.Wires.Add(w);
                }
            }
        } else {
            wire->SetMaterial(0, HighlightUnpowered->GetMaterial());
        }
    }

    ModeHighlight.Wire = wire;
    ModeHighlight.Current = substation;
}

void UPowerOverlay::ConfirmModeHighlight() { // TODO this could be less messy I guess...
    switch (ModeHighlight.Mode) {
    case EPowerOverlayMode::None:
        return;
    case EPowerOverlayMode::TogglePower:
        ConfirmTogglePower();
        return;
    case EPowerOverlayMode::Connect:
        ConfirmConnect();
        return;
    case EPowerOverlayMode::Disconnect:
        ConfirmDisconnect();
        return;
    default:
        checkNoEntry();
    }
}
void UPowerOverlay::ConfirmTogglePower() const {
    if (!ModeHighlight.Current)
        return;

    const auto elec = ModeHighlight.Current->GetComponentByClass<UElectricComponent>();
    switch (elec->GetState()) {
    case PowerState::Powered: case PowerState::Unpowered:
        elec->SetDeactivated();
        elec->GetSubstation()->Network->RecomputeStats();
        break;
    case PowerState::Deactivated:
        elec->SetUnpowered();
        elec->GetSubstation()->Network->RecomputeStats();
        break;
    default:
        checkNoEntry();
    }
}
void UPowerOverlay::ConfirmConnect() {
    if (!ModeHighlight.Current)
        return;

    if (!ModeHighlight.Source) {
        ModeHighlight.Source = ModeHighlight.Current;
        ModeHighlight.Current = nullptr;
        return;
    }

    if (const auto elec = CheckElec(ModeHighlight.Current)) {
        check(ModeHighlight.Source->IsA(ASubstation::StaticClass()));
        Cast<ASubstation>(ModeHighlight.Source)->Connect(elec);
        return;
    }
    if (const auto substation = Cast<ASubstation>(ModeHighlight.Current)) {
        if (const auto elec = CheckElec(ModeHighlight.Source)) {
            substation->Connect(elec);
            return;
        }
        if (const auto substation2 = Cast<ASubstation>(ModeHighlight.Source)) {
            substation->Network->MergeNetwork(substation2->Network);
            return;
        }
    }
    checkNoEntry();
}
void UPowerOverlay::ConfirmDisconnect() const {
    if (ModeHighlight.Current) {
        Cast<ASubstation>(ModeHighlight.Current)->DisconnectFromNetwork();
    } else if (ModeHighlight.Wire) {
        if (const auto elec1 = ModeHighlight.Wire->GetStart()->GetComponentByClass<UElectricComponent>())
            elec1->GetSubstation()->Disconnect(elec1);
        else if (const auto elec2 = ModeHighlight.Wire->GetEnd()->GetComponentByClass<UElectricComponent>())
            elec2->GetSubstation()->Disconnect(elec2);
        else
            checkNoEntry();
    }
}

void UPowerOverlay::ResetModeHighlight(bool deactivate) {
    switch (ModeHighlight.Mode) {
    case EPowerOverlayMode::None:
        return;
    case EPowerOverlayMode::TogglePower:
        ResetTogglePower();
        break;
    case EPowerOverlayMode::Connect:
        ResetConnect(deactivate);
        break;
    case EPowerOverlayMode::Disconnect:
        ResetDisconnect(deactivate);
        break;
    default:
        checkNoEntry();
        return;
    }
    ModeHighlight.Mode = EPowerOverlayMode::None;
}
void UPowerOverlay::ResetTogglePower() {    
    if (ModeHighlight.Current) {    
        switch (ModeHighlight.Current->GetComponentByClass<UElectricComponent>()->GetState()) {
        case PowerState::Powered:
        case PowerState::Unpowered:
            ModeHighlight.Current->RemoveCondition(HighlightDeactivated);
            break;
        case PowerState::Deactivated:
            ModeHighlight.Current->RemoveCondition(HighlightPowered);
            break;
        case PowerState::Disconnected:
        case PowerState::Initial:
            checkNoEntry();
            break;
        }
        ModeHighlight.Current = nullptr;
    }
}
void UPowerOverlay::ResetConnect(bool deactivate) {
    if (ModeHighlight.Current) {
        ModeHighlight.Current->RemoveCondition(HighlightUnderCursor);
        ModeHighlight.Current = nullptr;
    }

    if (ModeHighlight.Source) {
        if (deactivate) {
            ModeHighlight.Source->RemoveCondition(HighlightUnderCursor);
            ModeHighlight.Source = nullptr;
        }

        if (ModeHighlight.Wire) {
            ModeHighlight.Wire->DestroyComponent();
            ModeHighlight.Wire = nullptr;
            for (const auto wire : ModeHighlight.Wires) {
                wire->SetMaterial(0, nullptr);
            }
            ModeHighlight.Wires.Empty();
        }
    }
}
void UPowerOverlay::ResetDisconnect(bool deactivate) {    
    if (ModeHighlight.Current) {
        for (UWireComponent* w : ModeHighlight.Wires)
            w->SetMaterial(0, nullptr);
        ModeHighlight.Wires.Empty();
        ModeHighlight.Wire = nullptr;
        ModeHighlight.Current = nullptr;
    } else if (ModeHighlight.Wire) {
        ModeHighlight.Wire->SetMaterial(0, nullptr);
        ModeHighlight.Wire = nullptr;
    }

    if (deactivate) {
        TInlineComponentArray<UWireComponent*> wires;
        ComponentHolder->GetComponents<UWireComponent>(wires, true);
        for (UWireComponent* wire : wires)
            wire->SetCollisionProfileName(CollisionProfiles::NoCollision);
    }
}

UElectricComponent* UPowerOverlay::CheckElec(const ABuilding* building) {    
    if (!building)
        return nullptr;

    UElectricComponent* elec = building->GetComponentByClass<UElectricComponent>();
    if (!elec)
        return nullptr;

    if (elec->GetState() == PowerState::Initial)
        return nullptr;

    return elec;
}

bool UPowerOverlay::CanBeConnected(ABuilding* one, ABuilding* two, UWireComponent*& oldWire) const {
    if (const UElectricComponent* elec = CheckElec(one)) {
        if (const ASubstation* substation = Cast<ASubstation>(two))
            return CanBeConnected(substation, elec, oldWire);
    } else if (const ASubstation* substation = Cast<ASubstation>(one)) {
        if (const UElectricComponent* elec2 = CheckElec(two))
            return CanBeConnected(substation, elec2, oldWire);
        else if (const ASubstation* substation2 = Cast<ASubstation>(two))
            return CanBeConnected(substation, substation2);
    }
    return false;
}

bool UPowerOverlay::CanBeConnected(const ASubstation* one, const ASubstation* two) {
    if (one->Network == two->Network)
        return false;
    if (FVector::Distance(one->GetActorLocation(), two->GetActorLocation()) > ElectricityNetwork::MAX_WIRE_DISTANCE)
        return false;
    return true;
}

bool UPowerOverlay::CanBeConnected(const ASubstation* substation, const UElectricComponent* elec, UWireComponent*& oldWire) const {
    if (elec->GetSubstation() == substation)
        return false;
    if (FVector::Distance(substation->GetActorLocation(), elec->GetOwner()->GetActorLocation()) > ElectricityNetwork::MAX_WIRE_DISTANCE)
        return false;

    if (elec->GetSubstation()) {
        TInlineComponentArray<UWireComponent*> wires;
        ComponentHolder->GetComponents<UWireComponent>(wires, true);
        for (UWireComponent* w : wires) {
            if ((w->GetStart() == elec->GetSubstation() && w->GetEnd() == elec->GetOwner()) ||
                (w->GetStart() == elec->GetOwner() && w->GetEnd() == elec->GetSubstation())) {
                oldWire = w;
                return true;
            }
        }
        checkNoEntry();
    }

    return true;
}

void UPowerOverlay::Toggle() {
    if (Active)
        Deactivate();
    else
        Activate();
}

void UPowerOverlay::Activate() {
    if (Active)
        return;

    const auto playerController = The::PlayerController(this);
    playerController->BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), playerController->BlueprintHolder->PowerOverlayUI);
    playerController->InputComponent->BindAction("Select", IE_Pressed, this, &UPowerOverlay::ConfirmModeHighlight);

    if (PowerUI == nullptr)
        PowerUI = CreateWidget<UPowerUI>(playerController, playerController->BlueprintHolder->PowerUIClass);   

    DoActivate();
}

void UPowerOverlay::Deactivate() {
    if (!Active)
        return;
    
    const auto playerController = The::PlayerController(this);
    playerController->Deselect();
    playerController->BlueprintHolder->PowerOverlayUI->Reset();
    playerController->InputComponent->RemoveActionBinding("Select", IE_Pressed);

    DoDeactivate();
}

void UPowerOverlay::OnNetworkUpdate() {
    if (Active) {
        DoDeactivate();
        DoActivate();
    }
}

void UPowerOverlay::AddDisconnected(const UElectricComponent* building) const {
    if (Active) {
        AddFloatingText(building);
        Highlight(building);
    }
}

void UPowerOverlay::DoActivate() {   
    const auto electricityManager = The::ElectricityManager(this);

    // Create a dummy actor that will hold all the cable components
    check(ComponentHolder == nullptr);
    ComponentHolder = GetWorld()->SpawnActor<AActor>();
    ComponentHolder->SetActorTickEnabled(false);
    USceneComponent* RootComponent = NewObject<USceneComponent>(ComponentHolder);
    RootComponent->SetMobility(EComponentMobility::Static);
    ComponentHolder->SetRootComponent(RootComponent);

    // handle network
    for (const ElectricityNetwork* network : electricityManager->ElectricityNetworks) {          
        for (const auto substation : network->Substations) {
            AddWires(substation);
            AddFloatingTexts(substation);
            HighlightBuildings(substation);
        }
    }

    // handle buildings not connected to a network
    for (const auto building : electricityManager->Disconnected) {
        check(building->GetState() == PowerState::Disconnected);
        AddFloatingText(building);
        Highlight(building);
    }

    Active = true;
}

void UPowerOverlay::DoDeactivate() {
    ResetModeHighlight(true);

    const auto electricityManager = The::ElectricityManager(this);

    // Remove Wires & floating text
    ComponentHolder->Destroy();
    ComponentHolder = nullptr;

    // remove highlights
    for (const auto network : electricityManager->ElectricityNetworks) {
        for (const auto substation : network->Substations) {
            for (const auto building : substation->ConnectedBuildings)
                RemoveHighlight(building);
            for (const auto habitat : substation->ConnectedHabitats)
                RemoveHighlight(habitat);
        }
    }
    for (const auto building : electricityManager->Disconnected)
        RemoveHighlight(building);

    Active = false;
}

void UPowerOverlay::AddWires(ASubstation* substation) const {
    // Connect to nearby Substations
    for (const auto otherSubstation : substation->Network->Substations) {
        if (substation == otherSubstation || FVector::Distance(substation->GetActorLocation(), otherSubstation->GetActorLocation()) > ElectricityNetwork::MAX_WIRE_DISTANCE)
            continue;
        // this will create 2 connections in the end, but that should be fine
        UWireComponent::Create(ComponentHolder, substation, otherSubstation);
    }

    // Connect to Buildings
    for (const auto building : substation->ConnectedBuildings)
        UWireComponent::Create(ComponentHolder, substation, building->GetOwner<ABuilding>());
    for (const auto habitat : substation->ConnectedHabitats)
        UWireComponent::Create(ComponentHolder, substation, habitat->GetOwner<ABuilding>());
}
void UPowerOverlay::AddFloatingTexts(const ASubstation* substation) const {
    AddFloatingPowerUI(substation);
    for (const auto building : substation->ConnectedBuildings)
        AddFloatingText(building);
    for (const auto habitat : substation->ConnectedHabitats) {
        AddFloatingText(habitat);
        for (const auto building : habitat->GetOwner<AHabitat>()->Buildings) {
            if (building->IsNonInteractable())
                continue;
            if (const auto elec = building->GetComponentByClass<UElectricComponent>())
                AddFloatingText(elec);
        }
    }
}
void UPowerOverlay::HighlightBuildings(const ASubstation* substation) const {    
    for (const auto building : substation->ConnectedBuildings)
        Highlight(building);
    for (const auto habitat : substation->ConnectedHabitats)
        Highlight(habitat);
}

void UPowerOverlay::AddFloatingPowerUI(const ASubstation* substation) const {
    const auto playerController = The::PlayerController(this);
    const auto ui = CreateWidget<UPowerUI>(playerController, playerController->BlueprintHolder->PowerUIClass);    
    if (substation->Network)
        ui->Set(substation->Network->GetTotalConstantProduction(), substation->Network->GetTotalConstantConsumption());
    else
        ui->Set(0, 0);
    AddFloatingWidget(substation->GetActorLocation(), ui);
}

void UPowerOverlay::AddFloatingText(const UElectricComponent* building) const {    
   const auto playerController = The::PlayerController(this);
   const auto ui = CreateWidget<UTextUI>(playerController, playerController->BlueprintHolder->TextUIClass);
   ui->Text->SetText(FText::AsNumber(-building->Consumption, &FNumberFormattingOptions::DefaultNoGrouping()));
   if (building->Consumption > 0)
       ui->Text->SetColorAndOpacity(PowerUI->GetConsumptionColor());
   else
       ui->Text->SetColorAndOpacity(PowerUI->GetProductionColor());
   AddFloatingWidget(building->GetOwner()->GetActorLocation(), ui);
}

void UPowerOverlay::AddFloatingWidget(FVector location, UUserWidget* widget) const {
    UScalingWidgetComponent* widgetComponent = NewObject<UScalingWidgetComponent>(ComponentHolder);
    widgetComponent->Init(widget, 500., 1./4.);
    widgetComponent->RegisterComponent();
    widgetComponent->AttachToComponent(ComponentHolder->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    widgetComponent->SetWorldLocation(location + FVector(0., 0. , 25.));
    ComponentHolder->AddInstanceComponent(widgetComponent);
}

void UPowerOverlay::Highlight(const UElectricComponent* building) const {    
    switch (building->GetState()) {
    case PowerState::Powered:
        building->GetOwner<ABuilding>()->AddCondition(HighlightPowered);
        break;
    case PowerState::Unpowered:
    case PowerState::Disconnected:
        building->GetOwner<ABuilding>()->AddCondition(HighlightUnpowered);
        break;
    case PowerState::Deactivated:
        building->GetOwner<ABuilding>()->AddCondition(HighlightDeactivated);
        break;
    case PowerState::Initial:
        checkNoEntry();
        break;
    }

    if (building->GetType() == UElectricComponent::Type::Habitat)
        for (const auto b : building->GetOwner<AHabitat>()->Buildings) {
            if (b->IsNonInteractable())
                continue;
            if (const auto elec = b->GetComponentByClass<UElectricComponent>())
                Highlight(elec);
        }
}

void UPowerOverlay::RemoveHighlight(const UElectricComponent* building) {
    const auto owner = building->GetOwner<ABuilding>();
    owner->RemoveCondition(HighlightPowered);
    owner->RemoveCondition(HighlightUnpowered);
    owner->RemoveCondition(HighlightDeactivated);

    if (building->GetType() == UElectricComponent::Type::Habitat)
        for (const auto b : building->GetOwner<AHabitat>()->Buildings) {
            if (b->IsNonInteractable())
                continue;
            if (const auto elec = b->GetComponentByClass<UElectricComponent>())
                RemoveHighlight(elec);
        }
}
