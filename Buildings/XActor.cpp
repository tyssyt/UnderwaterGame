// Fill out your copyright notice in the Description page of Project Settings.

#include "XActor.h"

UGameInstanceX* AXActor::GetGameInstance() const {
    return GetWorld()->GetGameInstance<UGameInstanceX>();
}

void AXActor::SetAllMaterials(UMaterialInterface* material) const {
    TInlineComponentArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes, true);
    for (auto mesh : meshes)
        for (int i=0; i < mesh->GetMaterials().Num(); ++i)
            mesh->SetMaterial(i, material);
}
