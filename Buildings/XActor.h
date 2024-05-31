// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XActor.generated.h"

UCLASS()
class XD_API AXActor : public AActor {
    GENERATED_BODY()

protected:
    UFUNCTION(BlueprintCallable)
    void SetAllMaterials(UMaterialInterface* material) const;
    UFUNCTION(BlueprintCallable)
    void SetOverlayMaterial(UMaterialInterface* material) const;
};
