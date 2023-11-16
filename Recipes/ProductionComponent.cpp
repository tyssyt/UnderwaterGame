// Fill out your copyright notice in the Description page of Project Settings.

#include "ProductionComponent.h"

#include "The.h"
#include "UI.h"
#include "Components/VerticalBoxSlot.h"
#include "XD/BlueprintHolder.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Buildings/Farm.h"
#include "XD/Inventory/InventoryUI.h"
#include "XD/Resources/ResourceAmountUI.h"

UProductionComponent::UProductionComponent() {
    PrimaryComponentTick.bCanEverTick = true;

    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void UProductionComponent::OnConstructionComplete(UConstructionOptions* options) {
    const auto& recipes = The::Encyclopedia(this)->GetRecipes(GetOwner()->GetClass());
    check(recipes.Num() > 0);
    if (recipes.Num() == 1)
        SetRecipe(recipes[0]);
}

void UProductionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!Recipe || !GetOwner()->IsActorTickEnabled())
        return;

    auto& inputs = Inventory->GetInputs();
    auto& outputs = Inventory->GetOutputs();
    check(Recipe->Ingredients.Num() == inputs.Num());
    check(Recipe->Results.Num() == outputs.Num());
    
    
    for (int idx=0; idx<outputs.Num(); idx++)
        if (outputs[idx].Current > 0)
            return; // no more space in output

    for (int idx=0; idx<inputs.Num(); idx++)
        if (inputs[idx].Current < Recipe->Ingredients[idx].amount)
            return; // not enough of ingredient

    for (int idx=0; idx<inputs.Num(); idx++)
        inputs[idx].PullFrom(Recipe->Ingredients[idx].amount);

    for (int idx=0; idx<outputs.Num(); idx++)
        outputs[idx].PushInto(Recipe->Results[idx].amount);
}

void UProductionComponent::SetRecipe(URecipe* recipe) {
    if (Recipe) {
        // TODO to undo recipes, we need to figure out what to do with the connected conveyors
        // TODO and items currently in the inventory
        return;
    }

    Recipe = recipe;
    Inventory->SetRecipe(recipe); // TODO maybe move the SetRecipe logic into here, no one else should need it?

    // TODO move this code to farm and have a generalised mechanism that allows to hook into SetRecipe
    if (const auto farm = GetOwner<AFarm>()) {
        farm->SetCrop(The::Encyclopedia(this)->GetCrop(recipe));
    }
}

void UProductionComponent::AddToSelectedUI(UBuildingSelectedUI* selectedUI) {
    const auto state = GetUIState();
    const auto ui = CreateUI(state, selectedUI->WidgetTree);
    const auto slot = selectedUI->Content->AddChildToVerticalBox(ui);
    slot->SetHorizontalAlignment(HAlign_Center);
    selectedUI->Storage->Data.Add(StaticClass(), NewObject<UProductionComponentSelectedData>()->Init(state, ui));
}

void UProductionComponent::UpdateSelectedUI(UBuildingSelectedUI* selectedUI) {
    const auto data = selectedUI->Storage->Get<UProductionComponentSelectedData>(StaticClass());
    check(data);

    const auto state = GetUIState();
    if (state != data->State) {
        const auto newChild = CreateUI(state, selectedUI->WidgetTree);

        // for some reason ReplaceChild or InsertChild at do not work, so this is a workaround
        //selectedUI->Content->ReplaceChild(data->UI, newChild);
        // TODO this will put things in the wrong order when there is another child in content
        const auto slot = selectedUI->Content->AddChildToVerticalBox(newChild);
        slot->SetHorizontalAlignment(HAlign_Center);
        selectedUI->Content->RemoveChild(data->UI);

        data->State = state;
        data->UI = newChild;
        return;
    }

    if (state == UIState::Normal) {
        Cast<UInventoryUI>(data->UI)->Tick();
    }
}

UProductionComponent::UIState UProductionComponent::GetUIState() const {
    if (!Recipe)
        return UIState::NotSelected;
    if (Recipe->Ingredients.IsEmpty())
        return UIState::NoInputs;
    return UIState::Normal;
}

UWidget* UProductionComponent::CreateUI(UIState state, UWidgetTree* tree) const {
    const auto bpHolder = The::BPHolder(this);
    switch (state) {
    case UIState::NotSelected: {
        const auto button = UX::CreateImageButton(
            tree,
            LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Images/Gear")),
            FVector2d(80., 80.),
            false
        );
        button->OnClicked.AddDynamic(this, &UProductionComponent::OpenRecipeSelector);
        return button;
    }
    case UIState::NoInputs: {
        // TODO maybe, if there are more then 3 or so use the smaller size for UI
        const auto box = tree->ConstructWidget<UWrapBox>();
        for (const auto& result : Recipe->Results)
            box->AddChildToWrapBox(CreateWidget<UResourceAmountUI>(tree, bpHolder->ResourceAmountUIClass)->Init(
                result.amount,
                result.resource
            ));
        return box;
    }
    case UIState::Normal: {
        return CreateWidget<UInventoryUI>(tree, bpHolder->InventoryUIClass)->Init(Inventory);
    }
    default:
        checkNoEntry();
        return nullptr;
    }
}

void UProductionComponent::OpenRecipeSelector() {
    const auto recipeSelector = CreateWidget<URecipeSelectorUI>(
        The::PlayerController(this),
        The::BPHolder(this)->RecipeSelectorUIClass
    )->Init(
        The::Encyclopedia(this)->GetRecipes(GetOwner()->GetClass()),
        [this](URecipe* recipe) { SetRecipe(recipe); }
    );
    recipeSelector->AddToViewport();
}

UProductionComponentSelectedData* UProductionComponentSelectedData::Init(UProductionComponent::UIState state, UWidget* ui) {
    State = state;
    UI = ui;
    return this;
}
