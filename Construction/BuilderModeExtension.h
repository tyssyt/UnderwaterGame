// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/ComponentX.h"
#include "XD/Buildings/Building.h"
#include "BuilderModeExtension.generated.h"

class UConstructionUI;

UCLASS(Abstract)
class XD_API UBuilderModeExtension : public UObject {
    GENERATED_BODY()

public:
    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) {}
    virtual void Update() {}
    virtual void End(bool cancelled) {}
};

UCLASS()
class XD_API UBuilderModeExtensions : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    UBuilderModeExtension* BuildingExtension;
    UPROPERTY()
    TMap<UComponentX*, UBuilderModeExtension*> ComponentExtensions;
};