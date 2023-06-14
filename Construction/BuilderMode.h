#pragma once

#include "ArrowMover.h"
#include "Components/WidgetInteractionComponent.h"
#include "XD/CameraPawn.h"
#include "XD/Buildings/Building.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Buildings/IndoorBuilding.h"

#include "UObject/Class.h"
#include "XD/PlayerControllerX.h"
#include "BuilderMode.generated.h"

UCLASS(Abstract)
class XD_API UBuilderMode : public UObject {
    GENERATED_BODY()

public:
    virtual bool Tick(const ACameraPawn& camera) {return true;}
    virtual UClass* IDK() {return nullptr;} //TODO better name or replace with a better solution
    virtual void Stop() {}
};

UCLASS()
class XD_API UBuildingBuilderMode : public UBuilderMode {
    GENERATED_BODY()

    // TODO have some form of alignment, i.e. to Neighbors or a global grid

    bool HasOverlap = false;

    enum Phase { Positioning, Waiting, Done };

    Phase Phase = Positioning;
    
    UPROPERTY()
    UMaterialInstanceDynamic* HighlightMaterial;
    UPROPERTY()
    UConstructionPlan* ConstructionPlan;
    UPROPERTY()
    ABuilding* Preview;

    UPROPERTY()
    UWidgetComponent* ConfirmSymbol;
    UPROPERTY()
    UWidgetComponent* CancelSymbol;
    UPROPERTY()
    UArrowMover* ArrowForward;
    UPROPERTY()
    UArrowMover* ArrowSideways;
    UPROPERTY()
    UArrowMover* ArrowRotate;

    void Position(const ACameraPawn& camera) const;
    void Wait() const;
    void CheckOverlap();
    void ConfirmPosition();
    UFUNCTION()
    void OnClickConfirm();
    UFUNCTION()
    void OnClickCancel();
    
    void RemoveBindingsWaiting() const;

    void PushUp();
    void PushDown();
    void PushLeft();
    void PushRight();
    void RotateLeft();
    void RotateRight();
    void Rotate90();

public:
    UBuildingBuilderMode();
    UBuildingBuilderMode* Init(UConstructionPlan* constructionPlan, UWorld* world);
    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop() override;
};

UCLASS()
class XD_API UConveyorBuilderMode : public UBuilderMode {
    GENERATED_BODY()

    // TODO have some form of alignment, i.e. to Neighbors or a global grid
    // TODO keyboard nudging via wasd after target is selected
    // TODO height adjustment via keys, both before and after target selection

    bool Done = false;
    bool HasOverlap = false;
    
    UPROPERTY()
    UMaterialInstanceDynamic* RedMaterial;
    UPROPERTY()
    UMaterialInstanceDynamic* GreenMaterial;

    struct SourceTarget { 
        enum class EType {NotSet, Building, ConveyorLink, ConveyorNode};

        EType Type = EType::NotSet;
        ABuilding* Building;

        // the following properties are only set if this is a Spliltter/Merger
        AConveyor* Conveyor;
        UStaticMeshComponent* ConveyorComponent;
    };
        
    
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
    UConveyorBuilderMode* Init();
    
    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop() override;

    void RemoveNode(UConveyorNode* node);

private:
    void CreateNextLinkAndNode();
    void TickSelectSource(const ACameraPawn& camera);
    void TickSelectNextPoint(const ACameraPawn& camera);
    void TickInsertNode(const ACameraPawn& camera);
    static FVector ProjectOntoLink(FVector loc, UConveyorLink* link);
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

UCLASS()
class XD_API UIndoorBuilderMode : public UBuilderMode {
    GENERATED_BODY()

    bool Buildable = false;
    
    UPROPERTY()
    UMaterialInstanceDynamic* HighlightMaterial;
    UPROPERTY()
    UConstructionPlan* ConstructionPlan;
    UPROPERTY()
    AIndoorBuilding* Preview;

public:
    UIndoorBuilderMode();
    UIndoorBuilderMode* Init(UConstructionPlan* constructionPlan, UWorld* world);

    virtual bool Tick(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
    virtual void Stop() override;

private:
    void ConfirmPosition();
    void SetInvisible();
    void SetNotBuildable();
    void SetBuildable();

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
