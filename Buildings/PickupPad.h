// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "CoreMinimal.h"
#include "XD/Inventory/InventoryComponent.h"
#include "PickupPad.generated.h"

UCLASS()
class XD_API APickupPad : public ABuilding {
    GENERATED_BODY()

protected:
    TArray<Material> ReservedMaterials;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UInventoryComponent* Inventory;

    APickupPad();

    void Reserve(const Material& material) { Material::AddTo(ReservedMaterials, material); }
    void Unreserve(const Material& material) { Material::RemoveFrom(ReservedMaterials, material); }
    int GetUnreserved(const UResource* resource) const;

    virtual void OnConstructionComplete(UBuilderModeExtensions* extensions) override;
    virtual void OnDismantle() override;
};
