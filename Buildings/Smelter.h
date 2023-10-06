// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/SelectedUI.h"
#include "XD/Inventory/InventorySlotUI.h"
#include "XD/Inventory/InventoryComponent.h"
#include "XD/Electricity/ElectricComponent.h"
#include "XD/Recipes/Recipe.h"
#include "XD/Recipes/RecipeSelectorUI.h"

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Smelter.generated.h"

UCLASS()
class XD_API ASmelter : public ABuilding {
    GENERATED_BODY()

public:
    ASmelter();

    UPROPERTY(EditAnywhere)
    UInventoryComponent* Inventory;

    UPROPERTY(EditAnywhere)
    UElectricComponent* Electricity;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    URecipe* activeRecipe;

    void SetRecipe(URecipe* recipe);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

};


UCLASS(Abstract)
class XD_API USmelterUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Input;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Output;

public:
    ASmelter* Smelter;

    virtual void Tick() override;

    UFUNCTION(BlueprintCallable)
    void OnClickRecipeSelect(URecipeSelectorUI* recipeSelectorUI);

};
