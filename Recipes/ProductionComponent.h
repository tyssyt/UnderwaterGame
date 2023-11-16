// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Recipe.h"
#include "Blueprint/WidgetTree.h"
#include "XD/ComponentX.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Inventory/InventoryComponent.h"
#include "ProductionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UProductionComponent : public UComponentX {
    GENERATED_BODY()

public:
    enum class UIState { NotSelected, NoInputs, Normal };

protected:    
    UPROPERTY()
    URecipe* Recipe;
    UPROPERTY()
    UInventoryComponent* Inventory;

    UIState GetUIState() const;
    UWidget* CreateUI(UIState state, UWidgetTree* tree) const;
    UFUNCTION()
    void OpenRecipeSelector();

public:
    UProductionComponent();
    virtual void OnConstructionComplete(UConstructionOptions* options) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SetRecipe(URecipe* recipe);

    virtual void AddToSelectedUI(UBuildingSelectedUI* selectedUI) override;
    virtual void UpdateSelectedUI(UBuildingSelectedUI* selectedUI) override;
};

UCLASS()
class XD_API UProductionComponentSelectedData : public USelectedUIData {
    GENERATED_BODY()

public:
    UProductionComponent::UIState State;
    UPROPERTY()
    UWidget* UI;

    UProductionComponentSelectedData* Init(UProductionComponent::UIState state, UWidget* ui);
};
