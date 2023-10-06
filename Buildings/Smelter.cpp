// Fill out your copyright notice in the Description page of Project Settings.

#include "Smelter.h"

#include "XD/GameInstanceX.h"
#include "XD/Utils.h"
#include "XD/Recipes/Recipe.h"

ASmelter::ASmelter() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/SM_Smelter"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Electricity = CreateDefaultSubobject<UElectricComponent>(TEXT("Electricity"));
    Electricity->Consumption = 100;
}

void ASmelter::BeginPlay() {
    Super::BeginPlay();
}


void ASmelter::Tick(float DeltaTime) {
    if (!activeRecipe)
        return;

    FInventorySlot* input = &Inventory->GetInputs()[0];
    FInventorySlot* output = &Inventory->GetOutputs()[0];

    if (input->Current < activeRecipe->Ingredients[0].amount || output->Current > 0)
        return;

    input->PullFrom(activeRecipe->Ingredients[0].amount);
    output->PushInto(activeRecipe->Results[0].amount);
}

void ASmelter::SetRecipe(URecipe* recipe) {
    // TODO drop items, disconnect conveyors etc.
    check(recipe->HasSize(1,1));
    activeRecipe = recipe;
    Inventory->SetRecipe(recipe);
}


void USmelterUI::Tick() {
    if (!Smelter)
        return;

    if (!Smelter->activeRecipe) {
        Input->Set(nullptr);
        Output->Set(nullptr);
        return;
    }

    Input->Set(&Smelter->Inventory->GetInputs()[0]);
    Output->Set(&Smelter->Inventory->GetOutputs()[0]);
}

void USmelterUI::OnClickRecipeSelect(URecipeSelectorUI* recipeSelectorUI) {
    if (Smelter) {
        recipeSelectorUI->SetRecipes(
            The::Encyclopedia(Smelter)->GetRecipes(ASmelter::StaticClass()),
            [s = this->Smelter](URecipe* recipe) {
                s->SetRecipe(recipe);
            }
        );
    }
}
