// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuilderMode.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/Conveyor.h"
#include "ConveyorBuilderMode.generated.h"

class UButtonWorkaround;

UCLASS()
class XD_API UConveyorBuilderMode : public UBuilderMode {
    GENERATED_BODY()

public:
    struct SourceTarget { 
        enum class EType {NotSet, Building, ConveyorLink, ConveyorNode};

        EType Type = EType::NotSet;
        ABuilding* Building;

        // the following properties are only set if this is a Spliltter/Merger
        AConveyor* Conveyor;
        UStaticMeshComponent* ConveyorComponent;
    };

private:

    // TODO have some form of alignment, i.e. to Neighbors or a global grid
    // TODO height adjustment via keys, both before and after target selection

    bool Done = false;
    bool HasOverlap = false;
    
    UPROPERTY()
    UMaterialInstance* RedMaterial;
    UPROPERTY()
    UMaterialInstance* GreenMaterial;
    UPROPERTY()
    UTexture2D* ConveyorImage;        
    
    SourceTarget Source;
    SourceTarget Target;
    SourceTarget CurrentHighlight;
    bool CurrentHighlightValid = false;;
    UPROPERTY()
    const UResource* Resource;
    
    UPROPERTY()
    AActor* Preview;
    
    
    UPROPERTY()
    FVector NextLinkStartPos;
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
    UConveyorBuilderMode();
    UConveyorBuilderMode* Init(UConstructionPlan* constructionPlan);
    
    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop(bool success) override;

    void RemoveNode(UConveyorNode* node);

private:
    void CreateNextLinkAndNode();
    void TickSelectSource(const ACameraPawn& camera);
    void ComputeCostSelectSource() const;
    void TickSelectNextPoint(const ACameraPawn& camera);
    void ComputeCostSelectNextPoint() const;
    void TickInsertNode(const ACameraPawn& camera);
    void ComputeCostInsertNode() const;
    static FVector ProjectOntoLink(FVector loc, const UConveyorLink* link);
    ABuilding* SpawnSplitter(bool isSource, const UResource* resource) const;
    bool HighlightUnderCursor(const APlayerControllerX* playerController, bool isSource);
    void CheckOverlap();
    bool CheckOverlapLinks();
    bool CheckOverlapNodes();
    bool CheckOverlap(UStaticMeshComponent* mesh, std::vector<UStaticMeshComponent*> allowedMeshes, std::vector<SourceTarget*> allowed) const;
    void AddArrowsToNode(UConveyorNode* node, UTexture2D* cancelTexture);

    void Confirm();
    void SetHighlight(ABuilding* building, bool valid);
    void RemoveCurrentHighlight();
    void DrawNextLink(FVector target, bool drawNode) const;

    UFUNCTION()
    void UpdateLinks(UConveyorNode* node);
    UFUNCTION()
    void OnClickConfirm();
    UFUNCTION()
    void OnClickCancel();

    static bool CheckValidSource(const ABuilding* building);
    const UResource* CheckValidTarget(const ABuilding* building) const;
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
