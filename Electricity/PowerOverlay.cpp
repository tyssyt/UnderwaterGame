// Fill out your copyright notice in the Description page of Project Settings.

#include "PowerOverlay.h"

#include "ElectricityManager.h"
#include "The.h"
#include "XD/BlueprintHolder.h"
#include "XD/CameraPawn.h"
#include "XD/CollisionProfiles.h"
#include "XD/PlayerControllerX.h"

UPowerOverlay::UPowerOverlay() : Active(false) {
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatPoweredFinder(TEXT("/Game/Assets/Materials/GhostMaterials/PowerOverlay_Powered"));
    MatPowered = MatPoweredFinder.Object;
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatUnpoweredFinder(TEXT("/Game/Assets/Materials/GhostMaterials/PowerOverlay_Unpowered"));
    MatUnpowered = MatUnpoweredFinder.Object;
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatDeactivatedFinder(TEXT("/Game/Assets/Materials/GhostMaterials/PowerOverlay_Deactivated"));
    MatDeactivated = MatDeactivatedFinder.Object;    
    const static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatHighlightFinder(TEXT("/Game/Assets/Materials/GhostMaterials/PowerOverlay_Highlight"));
    MatHighlight = MatHighlightFinder.Object;    
}

void UPowerOverlay::Tick(float DeltaTime) {    
    // TODO the LastFrameNumberWeTicked is copy pasted from the tutorial on ticking, should I put this in other places as well?
    if (LastFrameNumberWeTicked == GFrameCounter)
        return;
    LastFrameNumberWeTicked = GFrameCounter;

    if (!Active)
        return;
    
    // Scale texts
    const FVector cameraLocation = The::CameraPawn(this)->GetActorLocation();
    TInlineComponentArray<UWidgetComponent*> widgetComponents;
    ComponentHolder->GetComponents<UWidgetComponent>(widgetComponents, true);
    for (UWidgetComponent* widgetComponent : widgetComponents)
        ScaleFloatingWidget(widgetComponent, cameraLocation);
    
    const EMode mode = The::BPHolder(this)->PowerOverlayUI->GetMode();
    if (mode != ModeHighlight.Mode) {
        ResetModeHighlight(true);
        ModeHighlight.Mode = mode;

        if (mode == EMode::Disconnect) {
            // set collision of wires
            TInlineComponentArray<UWireComponent*> wires;
            ComponentHolder->GetComponents<UWireComponent>(wires, true);
            for (UWireComponent* wire : wires)
                wire->SetCollisionProfileName(CollisionProfiles::UI);
        }
    }

    switch (mode) {
    case EMode::None:
        return;
    case EMode::TogglePower:
        TickTogglePower();
        break;
    case EMode::Connect:
        TickConnect();
        break;
    case EMode::Disconnect:
        TickDisconnect();
        break;
    default:
        checkNoEntry();
    }
}

void UPowerOverlay::TickTogglePower() {    
    const UElectricComponent* elec = CheckElec(The::PlayerController(this)->GetUnderCursor<ABuilding>());

    // can't toggle disconnected Buildings
    if (elec && elec->GetState() == PowerState::Disconnected)
        elec = nullptr;

    // can't toggle power producers
    if (elec && elec->Consumption <= 0)
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
            underCursor->SetAllMaterials(MatDeactivated);
            break;
        case PowerState::Deactivated:
            underCursor->SetAllMaterials(MatPowered);
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
    if (underCursor && underCursor->constructionState != EConstructionState::Done)
        underCursor = nullptr;

    if (!ModeHighlight.Source) {
        if (!CheckElec(underCursor) && !Cast<ASubstation>(underCursor))
            underCursor = nullptr;

        if (underCursor == ModeHighlight.Current)
            return;

        ResetConnect(false);

        if (underCursor)
            underCursor->SetAllMaterials(MatHighlight);

        ModeHighlight.Current = underCursor;
    } else {
        UWireComponent* oldLink = nullptr;
        if (underCursor && !CanBeConnected(ModeHighlight.Source, underCursor, oldLink))
            underCursor = nullptr;
        
        if (underCursor == ModeHighlight.Current)
            return;

        ResetConnect(false);

        if (underCursor) {
            underCursor->SetAllMaterials(MatHighlight);
            ModeHighlight.Current = underCursor;
            ModeHighlight.Wire = UWireComponent::Create(ComponentHolder, ModeHighlight.Source, underCursor);
            ModeHighlight.Wire->SetMaterial(0, MatPowered);
            if (oldLink) {
                oldLink->SetMaterial(0, MatUnpowered);
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
                    w->SetMaterial(0, MatUnpowered);
                    ModeHighlight.Wires.Add(w);
                }
            }
        } else {
            wire->SetMaterial(0, MatUnpowered);
        }
    }

    ModeHighlight.Wire = wire;
    ModeHighlight.Current = substation;
}

void UPowerOverlay::ConfirmModeHighlight() { // TODO this could be less messy I guess...
    switch (ModeHighlight.Mode) {
    case EMode::None:
        return;
    case EMode::TogglePower: {
        if (!ModeHighlight.Current)
            return;
        UElectricComponent* elec = ModeHighlight.Current->GetComponentByClass<UElectricComponent>();
        switch (elec->GetState()) {
        case PowerState::Powered:
        case PowerState::Unpowered:
            elec->SetState(PowerState::Deactivated);
            break;
        case PowerState::Deactivated:
            elec->SetState(PowerState::Unpowered);
            break;
        case PowerState::Initial:
        case PowerState::Disconnected: default: checkNoEntry();
        }
        elec->Substation->Network->RecomputeStats();
        return;
    }
    case EMode::Connect: {
        if (!ModeHighlight.Current)
            return;
        if (!ModeHighlight.Source) {
            ModeHighlight.Source = ModeHighlight.Current;
            ModeHighlight.Current = nullptr;
            return;
        }

        if (UElectricComponent* elec = CheckElec(ModeHighlight.Current)) {
            check(ModeHighlight.Source->IsA(ASubstation::StaticClass()));
            if (elec->Substation) {
                elec->Substation->Disconnect(elec);
            }
            Cast<ASubstation>(ModeHighlight.Source)->Connect(elec);
            return;
        }
        if (ASubstation* substation = Cast<ASubstation>(ModeHighlight.Current)) {
            if (UElectricComponent* elec2 = CheckElec(ModeHighlight.Source)) {
                if (elec2->Substation) {
                    elec2->Substation->Disconnect(elec2);
                }
                substation->Connect(elec2);
                return;
            }
            if (const ASubstation* substation2 = Cast<ASubstation>(ModeHighlight.Source)) {
                substation->Network->MergeNetwork(substation2->Network);
                return;
            }
        }
        checkNoEntry();
        return;
    }
    case EMode::Disconnect: {
        if (ModeHighlight.Current) {
            Cast<ASubstation>(ModeHighlight.Current)->DisconnectFromNetwork();
        } else if (ModeHighlight.Wire) {
            UElectricComponent* elec = ModeHighlight.Wire->GetStart()->GetComponentByClass<UElectricComponent>();
            if (!elec)
                elec = ModeHighlight.Wire->GetEnd()->GetComponentByClass<UElectricComponent>();
            elec->Substation->Disconnect(elec);
        }
        return;
    }
    default:
        checkNoEntry();
    }
}

void UPowerOverlay::ResetModeHighlight(bool deactivate) {
    switch (ModeHighlight.Mode) {
    case EMode::None:
        return;
    case EMode::TogglePower:
        ResetTogglePower();
        break;
    case EMode::Connect:
        ResetConnect(deactivate);
        break;
    case EMode::Disconnect:
        ResetDisconnect(deactivate);
        break;
    default:
        checkNoEntry();
        return;
    }
    ModeHighlight.Mode = EMode::None;
}
void UPowerOverlay::ResetTogglePower() {    
    if (ModeHighlight.Current) {
        Highlight(ModeHighlight.Current->GetComponentByClass<UElectricComponent>());
        ModeHighlight.Current = nullptr;
    }
}
void UPowerOverlay::ResetConnect(bool deactivate) {
    if (ModeHighlight.Current) {
        if (const UElectricComponent* elec = ModeHighlight.Current->GetComponentByClass<UElectricComponent>()) {
            Highlight(elec);
        } else {
            check(ModeHighlight.Current->IsA(ASubstation::StaticClass()));
            ModeHighlight.Current->SetAllMaterials(nullptr);
        }
        ModeHighlight.Current = nullptr;
    }

    if (ModeHighlight.Source) {
        if (deactivate) {
            // reset source
            if (const UElectricComponent* elec = ModeHighlight.Source->GetComponentByClass<UElectricComponent>()) {
                Highlight(elec);
            } else {
                check(ModeHighlight.Source->IsA(ASubstation::StaticClass()));
                ModeHighlight.Source->SetAllMaterials(nullptr);
            }
            ModeHighlight.Source = nullptr;
        }

        if (ModeHighlight.Wire) {
            ModeHighlight.Wire->DestroyComponent();
            ModeHighlight.Wire = nullptr;
            for (UWireComponent* wire : ModeHighlight.Wires) {
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
    if (elec->Substation == substation)
        return false;
    if (FVector::Distance(substation->GetActorLocation(), elec->GetOwner()->GetActorLocation()) > ElectricityNetwork::MAX_WIRE_DISTANCE)
        return false;

    if (elec->Substation) {
        TInlineComponentArray<UWireComponent*> wires;
        ComponentHolder->GetComponents<UWireComponent>(wires, true);
        for (UWireComponent* w : wires) {
            if ((w->GetStart() == elec->Substation && w->GetEnd() == elec->GetOwner()) ||
                (w->GetStart() == elec->GetOwner() && w->GetEnd() == elec->Substation)) {
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
        for (ASubstation* substation : network->substations) {
            AddWires(substation);
            AddFloatingTexts(substation);
            HighlightBuildings(substation);
        }
    }

    // handle buildings not connected to a network
    for (const UElectricComponent* building : electricityManager->Disconnected) {
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
    for (const ElectricityNetwork* network : electricityManager->ElectricityNetworks)
        for (const ASubstation* substation : network->substations)
            for (const UElectricComponent* building : substation->ConnectedBuildings)
                building->GetOwner<AXActor>()->SetAllMaterials(nullptr);

    for (const UElectricComponent* building : electricityManager->Disconnected)
        building->GetOwner<AXActor>()->SetAllMaterials(nullptr);

    Active = false;
}

void UPowerOverlay::AddWires(ASubstation* substation) const {        
    // Connect to nearby Substations
    for (ASubstation* otherSubstation : substation->Network->substations) {
        if (substation == otherSubstation || FVector::Distance(substation->GetActorLocation(), otherSubstation->GetActorLocation()) > ElectricityNetwork::MAX_WIRE_DISTANCE)
            continue;
        // this will create 2 connections in the end, but that should be fine
        UWireComponent::Create(ComponentHolder, substation, otherSubstation);
    }    

    // Connect to Buildings
    for (const UElectricComponent* building : substation->ConnectedBuildings)
        UWireComponent::Create(ComponentHolder, substation, building->GetOwner<ABuilding>());
}
void UPowerOverlay::AddFloatingTexts(const ASubstation* substation) const {
    AddFloatingPowerUI(substation);
    for (const UElectricComponent* building : substation->ConnectedBuildings)
        AddFloatingText(building);
}
void UPowerOverlay::HighlightBuildings(const ASubstation* substation) const {    
    for (const UElectricComponent* building : substation->ConnectedBuildings)
        Highlight(building);
}

void UPowerOverlay::AddFloatingPowerUI(const ASubstation* substation) const {
    const auto  playerController = The::PlayerController(this);
    UPowerUI* ui = CreateWidget<UPowerUI>(playerController, playerController->BlueprintHolder->PowerUIClass);    
    if (substation->Network)
        ui->Set(substation->Network->GetTotalConstantProduction(), substation->Network->GetTotalConstantConsumption());
    else
        ui->Set(0, 0);
    AddFloatingWidget(substation->GetActorLocation(), ui, playerController);
}

void UPowerOverlay::AddFloatingText(const UElectricComponent* building) const {    
   const auto  playerController = The::PlayerController(this);
   UTextUI* ui = CreateWidget<UTextUI>(playerController, playerController->BlueprintHolder->TextUIClass);
   ui->Text->SetText(FText::AsNumber(-building->Consumption, &FNumberFormattingOptions::DefaultNoGrouping()));
   if (building->Consumption > 0)
       ui->Text->SetColorAndOpacity(PowerUI->GetConsumptionColor());
   else
       ui->Text->SetColorAndOpacity(PowerUI->GetProductionColor());  
   AddFloatingWidget(building->GetOwner()->GetActorLocation(), ui, playerController);    
}

void UPowerOverlay::AddFloatingWidget(FVector location, UUserWidget* widget, const APlayerControllerX* playerController) const {
    // TODO copied from Buildemode
    UWidgetComponent* widgetComponent = NewObject<UWidgetComponent>(ComponentHolder);
    widgetComponent->RegisterComponent();
    widgetComponent->AttachToComponent(ComponentHolder->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    widgetComponent->SetWorldLocation(location + FVector(0., 0. , 25.));
    widgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    widgetComponent->SetWidget(widget);
    //widgetComponent->SetDrawSize(FVector2D(150, 75));
    widgetComponent->SetDrawAtDesiredSize(true);
    widgetComponent->SetPivot(FVector2D(0.5, 0.5));
    widgetComponent->SetGenerateOverlapEvents(false);
    widgetComponent->RecreatePhysicsState();
    ComponentHolder->AddInstanceComponent(widgetComponent);

    ScaleFloatingWidget(widgetComponent, playerController->GetPawn<ACameraPawn>()->GetActorLocation());
}

void UPowerOverlay::ScaleFloatingWidget(UWidgetComponent* widgetComponent, FVector cameraLocation) {
    static double FULL_SIZE_DISTANCE = 500.;
    static double CUTOFF_SCALE = 1./4.;
    
    const double distance = FVector::Distance(cameraLocation, widgetComponent->GetComponentLocation());
    const double scale = FMath::Min(FULL_SIZE_DISTANCE / distance, 1.);
        
    if (scale < CUTOFF_SCALE) {
        widgetComponent->SetVisibility(false);
        return;
    }

    widgetComponent->SetVisibility(true);
    // TODO the doc says that WidgetComponent at desired scale with constant resizing could be expensive...
    // TODO maybe calculate the "desired size" of the full scale widget, set that as size for this component and then scale the widget down inside
    widgetComponent->GetWidget()->SetRenderScale(FVector2d(scale));
}

void UPowerOverlay::Highlight(const UElectricComponent* building) const {    
    switch (building->GetState()) {
    case PowerState::Powered:
        building->GetOwner<AXActor>()->SetAllMaterials(MatPowered);
        break;
    case PowerState::Unpowered:
    case PowerState::Disconnected:
        building->GetOwner<AXActor>()->SetAllMaterials(MatUnpowered);
        break;
    case PowerState::Deactivated:
        building->GetOwner<AXActor>()->SetAllMaterials(MatDeactivated);
        break;
    case PowerState::Initial:
        checkNoEntry();
        break;
    }
}

UPowerOverlay::EMode UPowerOverlayUI::GetMode() const {
    if (CheckBoxTogglePower->GetCheckedState() == ECheckBoxState::Checked)
        return UPowerOverlay::EMode::TogglePower;
    if (CheckBoxConnect->GetCheckedState() == ECheckBoxState::Checked)
        return UPowerOverlay::EMode::Connect;
    if (CheckBoxDisconnect->GetCheckedState() == ECheckBoxState::Checked)
        return UPowerOverlay::EMode::Disconnect;
    return UPowerOverlay::EMode::None;
}

void UPowerOverlayUI::Reset() const {
    CheckBoxTogglePower->SetCheckedState(ECheckBoxState::Unchecked);
    CheckBoxConnect->SetCheckedState(ECheckBoxState::Unchecked);
    CheckBoxDisconnect->SetCheckedState(ECheckBoxState::Unchecked);
}
