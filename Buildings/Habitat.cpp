// Fill out your copyright notice in the Description page of Project Settings.

#include "Habitat.h"

#include "XD/GameInstanceX.h"

const float AHabitat::CELL_WIDTH = 10.f; // why c++ be like this?

static AIndoorBuilding* BLOCKED = reinterpret_cast<AIndoorBuilding*>(1);

AHabitat::AHabitat() : PopulationManager(new HabitatPopulationManager(this)),
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


    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;

        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/SM_Habitat")), BaseMaterial(TEXT("/Game/BasicShapeMaterial")) { }
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
    Mesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());


    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

AHabitat::~AHabitat() {
    delete PopulationManager;
}

void AHabitat::BeginPlay() {
    Super::BeginPlay();
    Inventory->GetInputs().Emplace(1000, GetGameInstance()->TheResourceBook->ConductiveOre);
}

void AHabitat::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    PopulationManager->TickPopulation();
}

AIndoorBuilding* AHabitat::getBuildingAt(int x, int y) const {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) {
        return BLOCKED;
    }
    return BuildGrid[x + (y * GRID_SIZE)];
}

void AHabitat::setBuildingAt(AIndoorBuilding* building, int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE || x + (y * GRID_SIZE) >= GRID_SIZE * GRID_SIZE) {
        return;
    }
    BuildGrid[x + (y * GRID_SIZE)] = building;
}

FVector AHabitat::getGridCellCenter(int x, int y) const {
    float xf = x + .5f - (GRID_SIZE / 2.f);
    float yf = y + .5f - (GRID_SIZE / 2.f);

    FVector local = FVector(xf * CELL_WIDTH, yf * CELL_WIDTH, 120.f);
    return GetActorRotation().RotateVector(local) + GetActorLocation();
}

std::pair<bool, Coordinate> AHabitat::findCoordinates(FVector hit) const {
    // project into local space
    FVector local = GetActorRotation().UnrotateVector(hit - GetActorLocation());

    UE_LOG(LogTemp, Warning, TEXT("z is: %f"), local.Z);

    // TODO reject bad z

    int x = FMath::RoundToInt((local.X + (CELL_WIDTH * GRID_SIZE / 2.f)) / CELL_WIDTH);
    int y = FMath::RoundToInt((local.Y + (CELL_WIDTH * GRID_SIZE / 2.f)) / CELL_WIDTH);

    return std::make_pair(true, std::make_pair(x, y));
}

bool AHabitat::canPlaceBuilding(AIndoorBuilding* building) const {
    int x = building->GridX;
    int y = building->GridY;
    for (Coordinate offset : *building->getGridOffsets()) {
        if (getBuildingAt(x + offset.first, y + offset.second)) {
            return false;
        }
    }
    return true;
}

void AHabitat::placeBuilding(AIndoorBuilding* building) {
    int x = building->GridX;
    int y = building->GridY;
    for (Coordinate offset : *building->getGridOffsets()) {
        setBuildingAt(building, x + offset.first, y + offset.second);
    }
    Buildings.Add(building);
    PopulationManager->NotifyBuildingsChanged();
}

void AHabitat::removeBuilding(AIndoorBuilding* building) {    
    int x = building->GridX;
    int y = building->GridY;
    for (Coordinate offset : *building->getGridOffsets()) {
        setBuildingAt(nullptr, x + offset.first, y + offset.second);
    }
    Buildings.Remove(building);
    PopulationManager->NotifyBuildingsChanged();
}



void UHabitatUI::Tick() {
    if (!Habitat)
        return;

    HabitatPopulationManager* populationManager = Habitat->PopulationManager;
    People->Set(populationManager->GetSettledPop(), populationManager->GetMaxPop(), Habitat->GetGameInstance()->TheResourceBook->People);
    Workforce->SetText(FText::AsNumber(populationManager->GetWorkforce()));
}
