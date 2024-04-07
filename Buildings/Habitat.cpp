// Fill out your copyright notice in the Description page of Project Settings.

#include "Habitat.h"

#include "The.h"
#include "Components/VerticalBoxSlot.h"
#include "XD/BlueprintHolder.h"
#include "XD/Encyclopedia/Encyclopedia.h"
#include "XD/Inventory/ConveyorGate.h"

const float AHabitat::CELL_WIDTH = 10.f; // why c++ be like this?

static AIndoorBuilding* BLOCKED = reinterpret_cast<AIndoorBuilding*>(1);

AHabitat::AHabitat() : PopulationManager(CreateDefaultSubobject<UHabitatPopulationManager>("PopManager")->Init(this)),
// @formatter:off
    BuildGrid{
        BLOCKED, BLOCKED, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, BLOCKED, BLOCKED,
        BLOCKED, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, BLOCKED,
        BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED,
        BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED,

        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED,

        BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED, BLOCKED,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,

        BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED,
        BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED,
        BLOCKED, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, BLOCKED,
        BLOCKED, BLOCKED, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, nullptr, nullptr, nullptr, BLOCKED, BLOCKED, BLOCKED, BLOCKED
    }
// @formatter:on
{
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/SM_Habitat"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRenderCustomDepth(true);
    SetRootComponent(Mesh);

    const auto conveyorGate = CreateDefaultSubobject<UConveyorGate>(TEXT("ConveyorGate"));
    conveyorGate->SetInput(true);
    conveyorGate->SetRelativeLocation(FVector(15., 0., 8.));
    conveyorGate->SetRenderCustomDepth(true);
    conveyorGate->SetupAttachment(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AHabitat::BeginPlay() {
    Super::BeginPlay();
}

void AHabitat::OnConstructionComplete(UConstructionOptions* options) {
    Super::OnConstructionComplete(options);

    for (const auto good : The::Encyclopedia(this)->FindGoods())
        Inventory->GetInputs().Emplace(1000, good);
}

void AHabitat::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    PopulationManager->TickPopulation();
}

AIndoorBuilding* AHabitat::GetBuildingAt(int x, int y) const {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) {
        return BLOCKED;
    }
    return BuildGrid[x + (y * GRID_SIZE)];
}

void AHabitat::SetBuildingAt(AIndoorBuilding* building, int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE || x + (y * GRID_SIZE) >= GRID_SIZE * GRID_SIZE) {
        return;
    }
    BuildGrid[x + (y * GRID_SIZE)] = building;
}

FVector AHabitat::GetGridCellCenter(int x, int y) const {
    const float xf = x + .5f - (GRID_SIZE / 2.f);
    const float yf = y + .5f - (GRID_SIZE / 2.f);

    const FVector local = FVector(xf * CELL_WIDTH, yf * CELL_WIDTH, 120.f);
    return GetActorRotation().RotateVector(local) + GetActorLocation();
}

TPair<bool, Coordinate> AHabitat::FindCoordinates(FVector hit) const {
    // project into local space
    const FVector local = GetActorRotation().UnrotateVector(hit - GetActorLocation());

    UE_LOG(LogTemp, Warning, TEXT("z is: %f"), local.Z);

    // TODO reject bad z

    const int x = FMath::RoundToInt((local.X + (CELL_WIDTH * GRID_SIZE / 2.f)) / CELL_WIDTH);
    const int y = FMath::RoundToInt((local.Y + (CELL_WIDTH * GRID_SIZE / 2.f)) / CELL_WIDTH);

    return MakeTuple(true, MakeTuple(x, y));
}

bool AHabitat::CanPlaceBuilding(AIndoorBuilding* building) const {
    const int x = building->GridX;
    const int y = building->GridY;
    for (const Coordinate offset : *building->GetGridOffsets()) {
        if (GetBuildingAt(x + offset.Key, y + offset.Value)) {
            return false;
        }
    }
    return true;
}

void AHabitat::PlaceBuilding(AIndoorBuilding* building) {
    int x = building->GridX;
    int y = building->GridY;
    for (const Coordinate offset : *building->GetGridOffsets())
        SetBuildingAt(building, x + offset.Key, y + offset.Value);
    Buildings.Add(building);
    PopulationManager->NotifyBuildingsChanged();
}

void AHabitat::RemoveBuilding(AIndoorBuilding* building) {    
    const int x = building->GridX;
    const int y = building->GridY;
    for (const Coordinate offset : *building->GetGridOffsets())
        SetBuildingAt(nullptr, x + offset.Key, y + offset.Value);
    Buildings.Remove(building);
    PopulationManager->NotifyBuildingsChanged();
}

bool AHabitat::HasBuilding(const UConstructionPlan* building) const {
    // TODO optimization: this can either be cached, or we can store buildings by type etc. pp
    for (const auto b : Buildings)
        if (b->GetClass() == building->BuildingClass)
            return true;
    return false;
}

void AHabitat::InitSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    components.Add(NewObject<UHabitatUI>(this)->Init(this));    
    Super::InitSelectedUI(components);
}

UHabitatUI* UHabitatUI::Init(AHabitat* habitat) {
    Habitat = habitat;
    return this;
}

void UHabitatUI::CreateUI(UBuildingSelectedUI* selectedUI) {
    Needs = CreateWidget<UNeedsSummaryUI>(selectedUI, The::BPHolder(Habitat)->NeedsSummaryUIClass);
    const auto needsSlot = selectedUI->Content->AddChildToVerticalBox(Needs);
    needsSlot->SetHorizontalAlignment(HAlign_Center);

    const auto populationManager = Habitat->PopulationManager;
    People = CreateWidget<UInventorySlotUI>(selectedUI, The::BPHolder(Habitat)->InventorySlotUIClass);
    People->Set(populationManager->GetCurrentPop(), populationManager->GetMaxPop(), The::Encyclopedia(Habitat)->People);
    const auto peopleSlot = selectedUI->Content->AddChildToVerticalBox(People);
    peopleSlot->SetHorizontalAlignment(HAlign_Center);
}

void UHabitatUI::Tick(UBuildingSelectedUI* selectedUI) {
    const auto populationManager = Habitat->PopulationManager;
    Needs->SetNeeds(populationManager);
    People->Set(populationManager->GetCurrentPop(), populationManager->GetMaxPop(), The::Encyclopedia(Habitat)->People);    
}
