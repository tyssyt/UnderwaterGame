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
#include "AssemblyLine.generated.h"

UCLASS()
class XD_API AAssemblyLine : public ABuilding {
    GENERATED_BODY()

public:
    AAssemblyLine();

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
class XD_API UAssemblyLineUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Input1;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Input2;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Output;

public:
    AAssemblyLine* AssemblyLine;

    virtual void Tick() override;

    UFUNCTION(BlueprintCallable)
    void OnClickRecipeSelect(URecipeSelectorUI* recipeSelectorUI);

};
