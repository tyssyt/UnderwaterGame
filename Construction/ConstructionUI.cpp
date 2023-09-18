// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionUI.h"

#include "ConstructionManager.h"
#include "XD/Buildings/ConstructionPlan.h"

void UConstructionUI::Set(const UConstructionPlan* constructionPlan, const UConstructionManager* constructionManager) {
    Set(constructionPlan->Name, constructionPlan->Image, constructionPlan->Materials, constructionManager);
}

void UConstructionUI::Set(const FText& name, UTexture2D* image, const std::vector<Material>& materials, const UConstructionManager* constructionManager) {
    BuildingName->SetText(name);
    BuildingImage->SetBrushFromTexture(image);
    TogglePower->SetVisibility(ESlateVisibility::Collapsed);

    Set(materials, constructionManager);
}

void UConstructionUI::Set(const std::vector<Material>& materials, const UConstructionManager* constructionManager) {
    Resources->ClearChildren();
    MaterialsToUpdate.Empty();
    for (auto const& material : materials) {
        UResourceBalanceUI* ui = CreateWidget<UResourceBalanceUI>(this, ResourceBalanceUIClass);
        ui->SetNeed(material.amount, material.resource);
        Resources->AddChild(ui);
        MaterialsToUpdate.Emplace(MaterialToUpdate{material.resource, ui});
    }
    UpdateHave(constructionManager);    
}

void UConstructionUI::UpdateHave(const UConstructionManager* constructionManager) {
    // TODO consider a solution with a single loop over constructionResources
    for (const auto& matUpdate : MaterialsToUpdate) {
        for (auto& resource : constructionManager->constructionResources) {
            if (matUpdate.Material == resource.Resource) {
                matUpdate.Ui->SetHave(resource.Total - resource.Reserved);
                break;
            }
        }
    }
}

UPanelSlot* UConstructionUI::AddExternalResource(UWidget* resource) const {
    return Resources->AddChild(resource);
}
