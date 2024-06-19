// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD/ComponentX.h"
#include "XD/Buildings/Building.h"
#include "XD/Buildings/BuildingSelectedUI.h"
#include "XD/Construction/BuilderModeExtension.h"
#include "XD/Resources/ResourceBalanceUI.h"
#include "WorkforceComponent.generated.h"

UCLASS()
class UNoWorkers : public UCondition {
    GENERATED_BODY()
public:
    UNoWorkers();
};

UENUM()
enum class EWorkforceState { Initial, /* TODO Deactivated,*/ Unstaffed, Staffed };

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XD_API UWorkforceComponent : public UComponentX {
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UCondition* Condition;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EWorkforceState State;

public:    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int Workers;

    UWorkforceComponent();

    UResource* GetWorkforce() const;

    EWorkforceState GetState() const { return State; }
    void SetStaffed();
    void SetUnstaffed();

    virtual UBuilderModeExtension* CreateBuilderModeExtension() override;
    virtual void OnConstructionComplete(UBuilderModeExtension* extension) override;
    virtual void OnDismantle() override;

    virtual void AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) override;
};

UCLASS()
class XD_API UWorkforceComponentUI : public UBuildingSelectedUIComponent {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UWorkforceComponent* WorkforceComponent;
    UPROPERTY()
    UResourceBalanceUI* UI;

public:
    UWorkforceComponentUI* Init(UWorkforceComponent* workforceComponent);
    virtual void CreateUI(UBuildingSelectedUI* selectedUI) override;
    virtual void Tick(UBuildingSelectedUI* selectedUI) override;
};

UCLASS()
class XD_API UWorkforceBuilderModeExtension : public UBuilderModeExtension {
    GENERATED_BODY()

    UPROPERTY()
    ABuilding* Preview;
    UPROPERTY()
    UResourceBalanceUI* ResourceUI;

public:
    virtual void Init(ABuilding* preview, UConstructionUI* constructionUI) override;
    virtual void Update() override;
};
