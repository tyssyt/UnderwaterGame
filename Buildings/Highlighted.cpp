// Fill out your copyright notice in the Description page of Project Settings.

#include "Highlighted.h"

FLinearColor UHighlighted::Blue(0x22 / 255.0f, 0x32 / 255.0f, 0xA0 / 255.0f, 0.7);
FLinearColor UHighlighted::Gray(0xAE / 255.0f, 0x1A / 255.0f, 0x10 / 255.0f, 0.7);
FLinearColor UHighlighted::Green(0x39 / 255.0f, 0xA9 / 255.0f, 0x22 / 255.0f, 0.8);
FLinearColor UHighlighted::Red(0xFF / 255.0f, 0x08 / 255.0f, 0x04 / 255.0f, 0.8);
FLinearColor UHighlighted::Yellow(0xFF / 255.0f, 0xF4 / 255.0f, 0x00 / 255.0f, 0.92);

UHighlighted::UHighlighted() {    
    const static ConstructorHelpers::FObjectFinder<UMaterial> GhostMaterialFinder(TEXT("/Game/Assets/Materials/GhostMaterials/GhostMaterial"));
    Material = GhostMaterialFinder.Object;
    Type = EType::Visual;
}

UHighlighted* UHighlighted::SetColor(FLinearColor color) {
    if (!Material->IsA<UMaterialInstanceDynamic>())
        Material = UMaterialInstanceDynamic::Create(Material, this);

    const auto mad = Cast<UMaterialInstanceDynamic>(Material);
    mad->SetVectorParameterValue("Color", color);
    return this;
}
