// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map.h"
#include "MapTokenUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/UniformGridPanel.h"
#include "Event/EventUI.h"
#include "MapGridUI.generated.h"

UCLASS(Abstract)
class XD_API UMapGridUI : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UTexture2D* ScoutSymbol;
    UPROPERTY()
    UTexture2D* EventSymbol;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UUniformGridPanel* Sectors;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCanvasPanel* Tokens;

    UPROPERTY()
    UScout* SelectedScout;    
    UPROPERTY()
    TArray<UWidget*> Waypoints;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMapTokenUI> UMapTokenUIClass;

    void AddAnchor() const;
    UMapTokenUI* GetToken(const UObject* Object) const;
    void ClickedScout(UScout* scout);

    virtual int32 NativePaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled
    ) const override;

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
    UMapGridUI(const FObjectInitializer& ObjectInitializer);
    UMapGridUI* Init();

    bool IsSelected(const UMapTokenUI* token) const;    
    void Deselect();

    void Clicked(UMapTokenUI* token);
    void RevealSector(int column, int row) const;

    void AddScout(UScout* scout);
    void AddEvent(UEventInstance* event);

    void UpdateWaypoints(UScout* scout);
};
