// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Recipe.h"
#include "XD/ComponentX.h"
#include "XD/Buildings/Building.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Inventory/InventoryComponent.h"
#include "ProductionComponent.generated.h"

UCLASS()
class UNoRecipeSelected : public UCondition {
    GENERATED_BODY()
public:
    UNoRecipeSelected();
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UProductionComponent : public UComponentX {
    GENERATED_BODY()

protected:    
    UPROPERTY()
    URecipe* Recipe;
    UPROPERTY()
    UInventoryComponent* Inventory;

public:
    UProductionComponent();
    virtual void OnConstructionComplete(UBuilderModeExtension* extension) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SetRecipe(URecipe* recipe);

    virtual void AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;

    friend class UProductionComponentUI;
};

UCLASS()
class XD_API UProductionComponentUI : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

protected:
    enum class UIState { NotSelected, NoInputs, Normal };
    UIState State;

    UPROPERTY()
    UProductionComponent* ProductionComponent;
    UPROPERTY()
    UWidget* UI;

    UIState GetUIState() const;
    UWidget* CreateWidgetFu(UIState state, UWidgetTree* tree) const;
    UFUNCTION()
    void OpenRecipeSelector();

public:
    UProductionComponentUI* Init(UProductionComponent* productionComponent);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;
};
