// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionMaterialsUI.h"

#include "UI.h"

void UConstructionMaterialsUI::AddMaterials(const TArray<Material>& materials) {
    for (const auto& mat : materials) {
        const auto ui = CreateWidget<UResourceBalanceUI>(this, ResourceBalanceUIClass);
        ui->SetNeed(mat.amount, mat.resource);
        Materials->AddChild(UI::Sized(this, ui, 60.f, 60.f));
        MaterialsToUpdate.Emplace(MaterialToUpdate{mat.resource, ui});
    }
}

void UConstructionMaterialsUI::Init(const TArray<Material>& materials, const UConstructionManager* constructionManager) {
    Clear();
    AddMaterials(materials);
    UpdateHave(constructionManager);
}

void UConstructionMaterialsUI::Clear() {
    MaterialsToUpdate.Empty();
    Materials->ClearChildren();
}

void UConstructionMaterialsUI::UpdateNeed(const TArray<Material>& materials, const UConstructionManager* constructionManager) {
    if (MaterialsToUpdate.Num() != materials.Num()) {
        Init(materials, constructionManager);
        return;
    }

    for (int idx = 0; idx<materials.Num(); ++idx) {
        const auto& newMat = materials[idx];
        auto& oldMat = MaterialsToUpdate[idx];
        check(newMat.resource == oldMat.Material);

        if (newMat.amount == oldMat.Ui->GetNeed())
            continue;

        oldMat.Material = newMat.resource;
        oldMat.Ui->SetNeed(newMat.amount, newMat.resource);
    }
    UpdateHave(constructionManager);
}

void UConstructionMaterialsUI::UpdateHave(const UConstructionManager* constructionManager) {
    // TODO consider a solution with a single loop over constructionResources
    for (const auto& matUpdate : MaterialsToUpdate) {
        for (auto& resource : constructionManager->ConstructionResources) {
            if (matUpdate.Material == resource.Resource) {
                matUpdate.Ui->SetHave(resource.Total - resource.Reserved);
                break;
            }
        }
    }
}

UPanelSlot* UConstructionMaterialsUI::AddExternalResource(UWidget* resource) const {
    return Materials->AddChild(UI::Sized(this, resource, 60.f, 60.f));
}
