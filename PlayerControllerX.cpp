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
    auto loadedData = ConfigLoader::Load(BlueprintHolder->MainUI->HotbarDock, GetWorld());
    GetGameInstance<UGameInstanceX>()->TheEncyclopedia = loadedData.Key;
    const auto encyclopedia = The::Encyclopedia(this);
    BlueprintHolder->EncyclopediaUI->Fill(encyclopedia, loadedData.Value);
    UConstructionManager* constructionManager = The::ConstructionManager(this);
    constructionManager->ConstructionResources->SetResources(encyclopedia->FindConstructionResources());

    // generate map
    {
        const auto map = The::Map(this);
        BlueprintHolder->MapUI->Init();

        const auto startChunk = map->CurrentChunk;
        constexpr int startSector = UMapChunk::CHUNK_SIZE/2;

        startChunk->Reveal(startSector,startSector);
        map->AddScout({ startChunk, startSector, startSector });

        int i = 5;
        for (const auto event : encyclopedia->GetAllEvents()) {
            UE_LOG(LogTemp, Error, TEXT("Creating Event %s at: (%i,%d)"), *event->Name.ToString(), i, 5);
            event->CreateInstance({startChunk, i++, 6});
        }
    }

    // replace pre placed Buildings
    {
        TArray<ABuilding*> buildings;
        for (const auto building : TActorRange<ABuilding>(GetWorld()))
            buildings.Add(building);

        for (const auto preplaced : buildings) {            
            UConstructionPlan* constructionPlan = nullptr;
            for (const auto c : encyclopedia->GetAllBuildings()) {
                if (c->BuildingClass == preplaced->GetClass()) {
                    constructionPlan = c;
                    break;
                }
            }
            if (!constructionPlan) {
                UE_LOG(LogTemp, Error, TEXT("Could not find construction Plan for preplaced building %s"), *preplaced->GetName());
                continue;
            }

            const auto newBuilding = ABuilding::Spawn(GetWorld(), constructionPlan);
            newBuilding->SetActorLocation(preplaced->GetActorLocation());
            newBuilding->SetActorRotation(preplaced->GetActorRotation());
            preplaced->Destroy();
        }
    }

    // generate initial resources
    {
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

        ++it;
        while (it) {
            constructionManager->AddPickupPad(*it);
            ++it;
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

void APlayerControllerX::Select(USelectedUI* ui) {    
    if (ui && SelectedUI != ui) {
        SelectedUI = ui;
        //		ui->Tick();
        BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), ui);
    }
}

void APlayerControllerX::UpdateSelected(AXActor* actor) {
    Select(BlueprintHolder->GetUI(actor));
}

void APlayerControllerX::Deselect() {
    SelectedUI = nullptr;
    BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), nullptr);
    BlueprintHolder->MainUI->HotbarDock->Reset();
}
