// Fill out your copyright notice in the Description page of Project Settings.

#include "HardOreMine.h"

#include "XD/GameInstanceX.h"
#include "XD/Utils.h"
#include "XD/Recipes/Recipe.h"

AHardOreMine::AHardOreMine() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Cylinder"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    Mesh->SetRelativeScale3D(FVector(1.f, 1.f, .25f));
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AHardOreMine::BeginPlay() {
    Super::BeginPlay();

    // TODO extract this logic to a component, which auto selects the recipe if it is just one and otherwise adds a button to the UI
    const auto& recipes = The::Encyclopedia(this)->GetRecipes(StaticClass());
    check(recipes.Num() == 1);
    const auto recipe = recipes[0];
    check(recipe->HasSize(0, 1));
    ProductionPerTick = recipe->Results[0].amount;
    Inventory->SetRecipe(recipe);
}

void AHardOreMine::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    Inventory->GetOutputs()[0].PushInto(ProductionPerTick);
}

void UHardOreMineUI::Tick() {
    if (Mine) {
        ProductionPerTick->SetText(FText::AsNumber(Mine->ProductionPerTick * 120)); // TODO use actual ticks/s
        ResourceImage->SetBrushFromTexture(Mine->Inventory->GetOutputs()[0].Resource->Image); // TODO save the resource somwhere else???
    }
}
