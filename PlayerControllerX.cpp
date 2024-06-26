// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerX.h"

#include "EngineUtils.h"
#include "GameInstanceX.h"
#include "The.h"
#include "Encyclopedia/ConfigLoader.h"
#include "Hotbar/HotbarDock.h"

APlayerControllerX::APlayerControllerX() {}

void APlayerControllerX::BeginPlay() {
    Super::BeginPlay();

    BlueprintHolder = NewObject<UBlueprintHolder>(this, BlueprintHolderClass);
    BlueprintHolder->Init(this);
    BlueprintHolder->MainUI->AddToViewport();

    // TODO there probably is a better place to put this, but for now it can live here
    auto loadedData = ConfigLoader::Load(BlueprintHolder->MainUI->HotbarDock);
    GetGameInstance<UGameInstanceX>()->TheEncyclopedia = loadedData.Key;
    UEncyclopedia* encyclopedia = The::Encyclopedia(this);
    BlueprintHolder->EncyclopediaUI->Fill(encyclopedia, loadedData.Value);
    UConstructionManager* constructionManager = The::ConstructionManager(this);
    constructionManager->ConstructionResources->SetResources(encyclopedia->FindConstructionResources());

    // generate initial resources
    for (const auto ship : TActorRange<ABuilderShip>(GetWorld())) 
        constructionManager->AddIdleBuilder(ship);
    
    TActorIterator<APickupPad> it(GetWorld());
    constructionManager->AddPickupPad(*it);
    
    int i = 0;
    for (const auto& startResource : encyclopedia->GetStartResources()) {
        auto& slot = it->Inventory->GetInputs()[i];
        slot.Current = startResource.Value;
        slot.Resource = startResource.Key;
        i++;
        if (i == 4) {
            i = 0;
            ++it;
            constructionManager->AddPickupPad(*it);
        }
    }
}

void APlayerControllerX::AcknowledgePossession(APawn* p) {
    Super::AcknowledgePossession(p);
    ShowMouseCursor(true);
}

void APlayerControllerX::ShowMouseCursor(bool showMouseCursor) {
    bShowMouseCursor = showMouseCursor;
    bEnableClickEvents = showMouseCursor;
    bEnableMouseOverEvents = showMouseCursor;
    SetIgnoreLookInput(showMouseCursor);

    if (showMouseCursor)
        SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
    else
        SetInputMode(FInputModeGameOnly());
}

void APlayerControllerX::TickUI() const {
    if (IsValid(SelectedUI))
        SelectedUI->Tick();
}

void APlayerControllerX::UpdateSelected(AXActor* actor) {
    USelectedUI* ui = BlueprintHolder->GetUI(actor);
    if (ui && SelectedUI != ui) {
        SelectedUI = ui;
        //		ui->Tick();
        BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), ui);
    }
}

void APlayerControllerX::Deselect() {
    SelectedUI = nullptr;
    BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), nullptr);
    BlueprintHolder->MainUI->HotbarDock->Reset();
}
