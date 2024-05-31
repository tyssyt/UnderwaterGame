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

class UConveyorGate;
class AMerger;
class ASplitter;
struct Material;

UCLASS()
class XD_API AConveyor : public ABuilding {
    GENERATED_BODY()

public:

    enum class ESourceTargetType { Building, ConveyorNode, ConveyorLink };

    static AConveyor* Create(
        UWorld* world,
        ABuilding* source,
        UConveyorGate* sourceGate,
        ABuilding* target,
        UConveyorGate* targetGate,
        const TArray<FVector>& nodes,
        UResource* resource
    );
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
    UConveyorGate* SourceGate;
    UPROPERTY(EditAnywhere)
    ABuilding* Target;
    UPROPERTY(EditAnywhere)
    UConveyorGate* TargetGate;
    UPROPERTY(EditAnywhere)
    TArray<FVector> Nodes;

    FInventorySlot* SourceInv;
    FInventorySlot* TargetInv;

protected:
    void MakeNode(const FVector& location);
    void MakeLink(const FVector& start, ESourceTargetType startType, const FVector& end, ESourceTargetType endType);

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(CallInEditor, Category="Conveyor")
    void Connect(UResource* resource = nullptr);
    virtual void OnDismantleStart() override;

    static bool HasCommonResource(UInventoryComponent* source, UInventoryComponent* target);
    static TSet<UResource*> FindCommonResources(UInventoryComponent* source, UInventoryComponent* target);
    static TArray<Material> ComputeCosts(const TArray<FVector>& nodes, ESourceTargetType splitter, ESourceTargetType merger, const UEncyclopedia* theEncyclopedia);
    static TArray<Material> ComputeCosts(double linkDist, int numNodes, ESourceTargetType splitter, ESourceTargetType merger, const UEncyclopedia* theEncyclopedia);

    TPair<AConveyor*, AConveyor*> SplitAt(UStaticMeshComponent* mesh, ABuilding* building);

};

UCLASS()
class XD_API UConveyorLink : public UStaticMeshComponent {
    GENERATED_BODY()

public:
    UConveyorLink();

    void Connect(FVector start, AConveyor::ESourceTargetType startType, FVector end, AConveyor::ESourceTargetType endType);
    
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
