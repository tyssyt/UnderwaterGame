// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructionManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "ConstructionMaterialsUI.generated.h"

UCLASS(Abstract)
class XD_API UConstructionMaterialsUI : public UUserWidget {
    GENERATED_BODY()

    struct MaterialToUpdate {
        UResource* Material;
        UResourceBalanceUI* Ui;
    };

    TArray<MaterialToUpdate> MaterialsToUpdate;

    void AddMaterials(const TArray<Material>& materials);

protected:    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Materials;

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceBalanceUI> ResourceBalanceUIClass;

    void Init(const TArray<Material>& materials, const UConstructionManager* constructionManager);
    void Clear();
    void UpdateNeed(const TArray<Material>& materials, const UConstructionManager* constructionManager);
    void UpdateHave(const UConstructionManager* constructionManager);
    UPanelSlot* AddExternalResource(UWidget* resource) const;
};
