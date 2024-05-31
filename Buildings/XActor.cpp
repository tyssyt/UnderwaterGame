// Fill out your copyright notice in the Description page of Project Settings.

#include "XActor.h"

void AXActor::SetAllMaterials(UMaterialInterface* material) const {
    TInlineComponentArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes, true);
    for (const auto mesh : meshes)
        for (int i=0; i < mesh->GetMaterials().Num(); ++i)
            mesh->SetMaterial(i, material);
}

void AXActor::SetOverlayMaterial(UMaterialInterface* material) const {
    TInlineComponentArray<UStaticMeshComponent*> meshes;
    GetComponents<UStaticMeshComponent>(meshes, true);
    for (const auto mesh : meshes)
        mesh->SetOverlayMaterial(material);
}
