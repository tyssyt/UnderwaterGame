// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/SelectedUI.h"

#include "Components/Image.h"
#include "CoreMinimal.h"
#include "XD/Buildings/XActor.h"
#include "ConductiveOreDeposit.generated.h"

UCLASS()
class XD_API AConductiveOreDeposit : public AXActor {
    GENERATED_BODY()

public:
    AConductiveOreDeposit();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;


protected:
    virtual void BeginPlay() override;

};


UCLASS(Abstract)
class XD_API UConductiveOreDepositUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    AConductiveOreDeposit* Deposit;

    virtual void Tick() override;

    UFUNCTION(BlueprintCallable)
    void OnClickConstruct();

};
