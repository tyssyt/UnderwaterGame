// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Resources/Resource.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "ConstructionUI.generated.h"

class UConstructionManager;
class UConstructionPlan;

UCLASS(Abstract)
class XD_API UConstructionUI : public UUserWidget {
    GENERATED_BODY()

    struct MaterialToUpdate {
        UResource* Material;
        UResourceBalanceUI* Ui;
    };

    TArray<MaterialToUpdate> MaterialsToUpdate;

protected:    

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* BuildingImage;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* BuildingName;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Resources;

public:    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* TogglePower;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UResourceBalanceUI> ResourceBalanceUIClass;

    void Set(const UConstructionPlan* constructionPlan, const UConstructionManager* constructionManager);
    void Set(const FText& name, UTexture2D* image, const std::vector<Material>& materials, const UConstructionManager* constructionManager);
    void Set(const std::vector<Material>& materials, const UConstructionManager* constructionManager);
    void UpdateHave(const UConstructionManager* constructionManager);
    UPanelSlot* AddExternalResource(UWidget* resource) const;
};
