// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XActor.h"
#include "Building.h"
#include "XD/SelectedUI.h"
#include "XD/Inventory/InventorySlot.h"
#include "XD/Inventory/InventorySlotUI.h"

#include "Components/TextBlock.h"
#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "XD/Encyclopedia/Encyclopedia.h"
#include "XD/Inventory/InventoryComponent.h"
#include "XD/Resources/Resource.h"
#include "Conveyor.generated.h"

struct Material;

UCLASS()
class XD_API AConveyor : public AXActor {
    GENERATED_BODY()

public:

    enum class ESourceTargetType { Building, ConveyorNode, ConveyorLink };

    static AConveyor* Create(UWorld* world, ABuilding* source, ABuilding* target, TArray<FVector> nodes, const UResource* resource);
    AConveyor();

    UPROPERTY(VisibleAnywhere)
    int Buffer = 0;
    UPROPERTY(VisibleAnywhere)
    int CurrentThroughput = 0;

    UPROPERTY(EditAnywhere)
    int MaxThroughput = 100;

    // TODO assert that Source can be pulled from, Target can be pushed into and that Source and Target have the same Item type
    UPROPERTY(EditAnywhere)
    ABuilding* Source;
    UPROPERTY(EditAnywhere)
    ABuilding* Target;
    UPROPERTY(EditAnywhere)
    TArray<FVector> Nodes;

    FInventorySlot* SourceInv;
    FInventorySlot* TargetInv;

protected:
    virtual void BeginPlay() override;

    void MakeNode(FVector location);
    void MakeLink(FVector start, FVector end);
    void DisconnectFromSplitter(class ASplitter* splitter) const;
    void DisconnectFromMerger(class AMerger* merger) const;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(CallInEditor, Category="Conveyor")
    void Connect(const UResource* resource = nullptr);

    static const UResource* FindCommonResource(UInventoryComponent* source, UInventoryComponent* target);
    static TArray<Material> ComputeCosts(FVector start, FVector* end, TArray<FVector>& nodes, ESourceTargetType splitter, ESourceTargetType merger, UEncyclopedia* theEncyclopedia);
    static TArray<Material> ComputeCosts(double linkDist, int numNodes, ESourceTargetType splitter, ESourceTargetType merger, UEncyclopedia* theEncyclopedia);

    std::pair<AConveyor*, AConveyor*> SplitAt(UStaticMeshComponent* mesh, ABuilding* building);

};

UCLASS()
class XD_API UConveyorLink : public UStaticMeshComponent {
    GENERATED_BODY()

public:
    UConveyorLink();

    void Connect(FVector start, FVector end);
    
};

UCLASS()
class XD_API UConveyorNode : public UStaticMeshComponent {
    GENERATED_BODY()

public:
    UConveyorNode();    
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
