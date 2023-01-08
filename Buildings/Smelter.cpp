// Fill out your copyright notice in the Description page of Project Settings.

#include "Smelter.h"

#include "XD/GameInstanceX.h"
#include "XD/Resources/ResourceBook.h"
#include "XD/Recipes/Recipe.h"

ASmelter::ASmelter() {
    PrimaryActorTick.bCanEverTick = true;

    // Structure to hold one-time initialization
    struct FConstructorStatics {
        ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
        ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
        FConstructorStatics() : PlaneMesh(TEXT("/Game/Assets/Meshes/SM_Smelter")), BaseMaterial(TEXT("/Game/BasicShapeMaterial")) {}
    };
    static FConstructorStatics ConstructorStatics;

    // Create static mesh component
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
    Mesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
    Mesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());

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

    if (input->Current < activeRecipe->ingredients[0].amount || output->Current > 0)
        return;

    // TODO use recipe cooking time
    input->PullFrom(activeRecipe->ingredients[0].amount);
    output->PushInto(activeRecipe->results[0].amount);
}

void ASmelter::SetRecipe(Recipe* recipe) {
    UE_LOG(LogTemp, Warning, TEXT("Recipe set"));
    // TODO drop items, disconnect conveyors etc.
    Inventory->GetInputs().Empty();
    Inventory->GetOutputs().Empty();

    // TODO sanity check recipe?
    activeRecipe = recipe;
    Inventory->GetInputs().Emplace(recipe->ingredients[0].amount * 2, activeRecipe->ingredients[0].resource);
    Inventory->GetOutputs().Emplace(recipe->results[0].amount * 2, activeRecipe->results[0].resource);
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
            &Smelter->GetGameInstance()->TheRecipeBook->SmelterRecipes,
            [s = this->Smelter](Recipe* recipe) {
                s->SetRecipe(recipe);
            }
        );
    }
}
