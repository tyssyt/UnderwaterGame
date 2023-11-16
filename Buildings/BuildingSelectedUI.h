// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingUI.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "XD/SelectedUI.h"
#include "BuildingSelectedUI.generated.h"

UCLASS(Abstract)
class XD_API USelectedUIData : public UObject {
    GENERATED_BODY()
};

UCLASS()
class XD_API USelectedUIStorage : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    TMap<UClass*, USelectedUIData*> Data;

    USelectedUIData* Get(const UClass* Class) const;
    template <class T>
    T* Get(const UClass* Class) const {
        return Cast<T>(Get(Class));
    }
};

UCLASS(Abstract)
class XD_API UBuildingSelectedUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UBuildingUI* Building;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Name;

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Top;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Content;

    UPROPERTY()
    ABuilding* Selected;

    UPROPERTY()
    USelectedUIStorage* Storage;
    
    UBuildingSelectedUI* Init(ABuilding* actor);

    virtual void Tick() override;
};
