// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/SelectedUI.h"
#include "XD/Components/InventoryComponent.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "CoreMinimal.h"
#include "XD/Buildings/XActor.h"
#include "OilFish.generated.h"

UCLASS()
class XD_API AOilFish : public AXActor {
    GENERATED_BODY()

public:
    AOilFish();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;


protected:
    virtual void BeginPlay() override;

};


UCLASS(Abstract)
class XD_API UOilFishUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* ResourceImage;

public:
    AOilFish* Deposit;

    virtual void Tick() override;

    UFUNCTION(BlueprintCallable)
    void OnClickConstruct();

};
