// Fill out your copyright notice in the Description page of Project Settings.

#include "AssemblyLine.h"

#include "The.h"
#include "XD/Encyclopedia/Encyclopedia.h"

AAssemblyLine::AAssemblyLine() {
    PrimaryActorTick.bCanEverTick = true;

    const static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Assets/Meshes/AssemblyLine"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
    Mesh->SetStaticMesh(MeshFinder.Object);
    SetRootComponent(Mesh);

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void AAssemblyLine::Tick(float DeltaTime) {
    if (!activeRecipe)
        return;

    FInventorySlot* input0 = &Inventory->GetInputs()[0];
    FInventorySlot* input1 = &Inventory->GetInputs()[1];
    FInventorySlot* output = &Inventory->GetOutputs()[0];

    if (input0->Current < activeRecipe->Ingredients[0].amount || input1->Current < activeRecipe->Ingredients[1].amount || output->Current > 0)
        return;

    input0->PullFrom(activeRecipe->Ingredients[0].amount);
    input1->PullFrom(activeRecipe->Ingredients[1].amount);
    output->PushInto(activeRecipe->Results[0].amount);
}

void AAssemblyLine::SetRecipe(URecipe* recipe) {
    // TODO drop items, disconnect conveyors etc.
    check(recipe->HasSize(2,1));
    activeRecipe = recipe;
    Inventory->SetRecipe(recipe);
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
        recipeSelectorUI->Init(
            The::Encyclopedia(AssemblyLine)->GetRecipes(AAssemblyLine::StaticClass()),
            [s = this->AssemblyLine](URecipe* recipe) {
                s->SetRecipe(recipe);
            }
        );
    }
}
