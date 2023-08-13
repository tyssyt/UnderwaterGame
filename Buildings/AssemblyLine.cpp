// Fill out your copyright notice in the Description page of Project Settings.

#include "AssemblyLine.h"

#include "XD/GameInstanceX.h"
#include "XD/Recipes/Recipe.h"

AAssemblyLine::AAssemblyLine() {
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh (TEXT("/Game/Assets/Meshes/AssemblyLine"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(PlaneMesh.Get());
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    Electricity = CreateDefaultSubobject<UElectricComponent>(TEXT("Electricity"));
    Electricity->Consumption = 100;
}

void AAssemblyLine::BeginPlay() {
    Super::BeginPlay();
}


void AAssemblyLine::Tick(float DeltaTime) {
    if (!activeRecipe)
        return;

    FInventorySlot* input0 = &Inventory->GetInputs()[0];
    FInventorySlot* input1 = &Inventory->GetInputs()[1];
    FInventorySlot* output = &Inventory->GetOutputs()[0];

    if (input0->Current < activeRecipe->ingredients[0].amount || input1->Current < activeRecipe->ingredients[1].amount || output->Current > 0)
        return;

    // TODO use recipe cooking time
    input0->PullFrom(activeRecipe->ingredients[0].amount);
    input1->PullFrom(activeRecipe->ingredients[1].amount);
    output->PushInto(activeRecipe->results[0].amount);
}

void AAssemblyLine::SetRecipe(Recipe* recipe) {
    UE_LOG(LogTemp, Warning, TEXT("Recipe set"));
    // TODO drop items, disconnect conveyors etc.
    Inventory->GetInputs().Empty();
    Inventory->GetOutputs().Empty();

    // TODO sanity check recipe?
    activeRecipe = recipe;
    Inventory->GetInputs().Emplace(recipe->ingredients[0].amount * 2, activeRecipe->ingredients[0].resource);
    Inventory->GetInputs().Emplace(recipe->ingredients[1].amount * 2, activeRecipe->ingredients[1].resource);
    Inventory->GetOutputs().Emplace(recipe->results[0].amount * 2, activeRecipe->results[0].resource);
}


void UAssemblyLineUI::Tick() {
    if (!AssemblyLine)
        return;

    if (!AssemblyLine->activeRecipe) {
        Input1->Set(nullptr);
        Input2->Set(nullptr);
        Output->Set(nullptr);
        return;
    }

    Input1->Set(&AssemblyLine->Inventory->GetInputs()[0]);
    Input2->Set(&AssemblyLine->Inventory->GetInputs()[1]);
    Output->Set(&AssemblyLine->Inventory->GetOutputs()[0]);
}

void UAssemblyLineUI::OnClickRecipeSelect(URecipeSelectorUI* recipeSelectorUI) {
    if (AssemblyLine) {
        recipeSelectorUI->SetRecipes(
            &AssemblyLine->GetGameInstance()->TheRecipeBook->AssemblyLineRecipes,
            [s = this->AssemblyLine](Recipe* recipe) {
                s->SetRecipe(recipe);
            }
        );
    }
}
