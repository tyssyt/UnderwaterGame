// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Building.h"
#include "XD/InventorySlot.h"
#include "XD/SelectedUI.h"
#include "XD/InventorySlotUI.h"

#include "Components/TextBlock.h"
#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Conveyor.generated.h"

UCLASS()
class XD_API AConveyor : public AActor {
    GENERATED_BODY()

public:
    static AConveyor* Create(UWorld* world, ABuilding* source, ABuilding* target, int maxThroughput);
    AConveyor();


    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    int Buffer = 0;
    UPROPERTY(VisibleAnywhere)
    int CurrentThroughput = 0;

    UPROPERTY(EditAnywhere)
    int MaxThroughput;

    // TODO assert that Source can be pulled from, Target can be pushed into and that Source and Target have the same Item type
    UPROPERTY(EditAnywhere)
    ABuilding* Source;
    UPROPERTY(EditAnywhere)
    ABuilding* Target;

    FInventorySlot* SourceInv;
    FInventorySlot* TargetInv;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(CallInEditor, Category="Conveyor")
    void Connect();

};

UCLASS(Abstract)
class XD_API UConveyorUI : public USelectedUI {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* CurrentThroughput;


    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Input;


    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UInventorySlotUI* Output;

public:
    AConveyor* Conveyor;

    virtual void Tick() override;

};
