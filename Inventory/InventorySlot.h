// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Resources/Resource.h"
#include "Engine/DataTable.h"
#include "CoreMinimal.h"

#include "InventorySlot.generated.h"

//TODO understand the whole UStruct thing better, I just threw random google results at it until I could edit the fields in the editor...

class AConveyor;

USTRUCT(BlueprintType)
struct XD_API FInventorySlot : public FTableRowBase {
    GENERATED_BODY()

public:
    FInventorySlot();
    explicit FInventorySlot(int max, const UResource* resource = nullptr, AConveyor* conveyor = nullptr);
    virtual ~FInventorySlot() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Current = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Max;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    const UResource* Resource;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AConveyor* Conveyor;

    int PushInto(int amount);
    int PullFrom(int amount);

};
