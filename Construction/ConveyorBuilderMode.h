// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuilderMode.h"
#include "Components/WidgetComponent.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Conveyor.h"
#include "ConveyorBuilderMode.generated.h"

class UButtonWorkaround;

UCLASS()
class XD_API UConveyorBuilderMode : public UBuilderMode {
    GENERATED_BODY()

    class SourceTarget {
        enum class EType {NotSet, Building, Junction, ConveyorLink, ConveyorNode};

        UConveyorBuilderMode* Parent;

        bool Valid = false;
        EType Type = EType::NotSet;
        ABuilding* Building = nullptr;

        // for Building
        UConveyorGate* Gate = nullptr;

        // for ConveyorLink & ConveyorNode
        AConveyor* Conveyor = nullptr;
        UStaticMeshComponent* ConveyorComponent = nullptr;

    public:
        explicit SourceTarget(UConveyorBuilderMode* const parent) : Parent(parent) {}

        bool IsSet() const { return Type != EType::NotSet; }
        bool IsValid() const { return IsSet() && Valid; }
        bool IsConveyorNode(const UConveyorNode* node) const { return Type == EType::ConveyorNode && ConveyorComponent == node; }
        bool IsConveyorLink(const UConveyorLink* link) const { return Type == EType::ConveyorLink && ConveyorComponent == link; }

        ABuilding* GetBuilding() const { return Building; }
        AConveyor* GetConveyor() const { return Conveyor; }
        UConveyorGate* GetGate() const { return Gate; }
        FVector GetLocation() const;
        AConveyor::ESourceTargetType ToConveyorType() const;

        void RemoveHighlight();
        void HighlightInvalid(ABuilding* building);
        void Highlight(UConveyorGate* gate, const TArray<UConveyorGate*>& otherGates);
        void Highlight(AJunction* junction);
        void Highlight(ABuilding* building, AConveyor* conveyor, UConveyorNode* node, bool valid);
        void Highlight(ABuilding* building, AConveyor* conveyor, UConveyorLink* link, bool valid);

        void Reset();
        UConstructionSite* CreateConstructionSite(const UEncyclopedia* encyclopedia) const;

        // TODO I don't like this function
        void GetOverlapIgnore(TArray<AActor*>& allowedActors, TArray<UStaticMeshComponent*>& allowedNodes) const;
    };


    // TODO have some form of alignment, i.e. to Neighbors or a global grid
    // TODO height adjustment via keys, both before and after target selection

    bool Done = false;
    bool HasOverlap = false;

    SourceTarget Source = SourceTarget(this);
    SourceTarget Target = SourceTarget(this);
    SourceTarget CurrentHighlight = SourceTarget(this);

    UPROPERTY()
    UResource* Resource;

    UPROPERTY()
    AActor* Preview;
    UPROPERTY()
    UInBuilderMode* Condition;

    UPROPERTY()
    UConveyorNode* NextNode;
    UPROPERTY()
    UConveyorLink* NextLink;
    UPROPERTY()
    TArray<UConveyorNode*> Nodes;
    UPROPERTY()
    TArray<UConveyorLink*> Links;

    UPROPERTY()
    UWidgetComponent* ConfirmSymbol;
    UPROPERTY()
    TArray<UButtonWorkaround*> ButtonWorkarounds;
    UPROPERTY()
    UConveyorLink* InsertNodeHoverLink;


public:
    UConveyorBuilderMode* Init(UConstructionPlan* constructionPlan);

    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop(bool cancelled) override;

    void RemoveNode(UConveyorNode* node);

private:
    void CreateNextLinkAndNode();
    void TickSelectSource(const ACameraPawn& camera);
    void ComputeCostSelectSource() const;
    void TickSelectNextPoint(const ACameraPawn& camera);
    void ComputeCostSelectNextPoint() const;
    void TickInsertNode(const ACameraPawn& camera);
    void ComputeCostInsertNode() const;
    static FVector ProjectOntoLink(const FVector& loc, const UConveyorLink* link);
    ABuilding* SpawnSplitter(bool isSource, UResource* resource) const;

    bool HighlightUnderCursor(const APlayerControllerX* playerController, bool isSource);
    bool HighlightConveyorNodeUnderCursor(AConveyor* conveyor, UConveyorNode* node, bool isSource);
    bool HighlightConveyorLinkUnderCursor(AConveyor* conveyor, UConveyorLink* link, const FVector& hitLoc, bool isSource);
    bool HighlightBuildingUnderCursor(ABuilding* building, const FVector& hitLoc, bool isSource);    

    void CheckOverlap();
    bool CheckOverlapLinks();
    bool CheckOverlapNodes();
    bool CheckOverlap(UStaticMeshComponent* mesh, const TArray<UStaticMeshComponent*>& allowedMeshes, const TArray<SourceTarget*>& allowed) const;
    void AddArrowsToNode(UConveyorNode* node, UTexture2D* cancelTexture);

    void Confirm();

    UFUNCTION()
    void UpdateLinks(UConveyorNode* node);
    UFUNCTION()
    void OnClickConfirm();
    UFUNCTION()
    void OnClickCancel();

    bool CheckValidBuilding(const ABuilding* building, bool isSource) const;
};

// Helper Class because the OnClick of the Button has no Parameters
UCLASS()
class XD_API UButtonWorkaround : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    UConveyorBuilderMode* BuilderMode;
    UPROPERTY()
    UConveyorNode* Parent;
    
    UFUNCTION()
    void OnClick();    
};
