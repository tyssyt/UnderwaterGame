// Fill out your copyright notice in the Description page of Project Settings.

#include "MapGridUI.h"

#include "The.h"
#include "UI.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/UniformGridSlot.h"
#include "Event/Event.h"
#include "XD/BlueprintHolder.h"

#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"

void UMapGridUI::AddAnchor() const {    
    const auto anchor = WidgetTree->ConstructWidget<UWidget>();
    Sectors->AddChildToUniformGrid(anchor, UMapChunk::CHUNK_SIZE-1, UMapChunk::CHUNK_SIZE-1);
}

UMapTokenUI* UMapGridUI::GetToken(const UObject* Object) const {    
    for (const auto token : Tokens->GetAllChildren())
        if (const auto tokenUI = Cast<UMapTokenUI>(token))
            if (tokenUI->Object == Object)
                return tokenUI;
    return nullptr;
}

UMapGridUI::UMapGridUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    const static ConstructorHelpers::FObjectFinder<UTexture2D> ScoutSymbolFinder(TEXT("/Game/Assets/WidgetBluePrints/Map/Scout"));
    ScoutSymbol = ScoutSymbolFinder.Object;
    const static ConstructorHelpers::FObjectFinder<UTexture2D> EventSymbolFinder(TEXT("/Game/Assets/WidgetBluePrints/Map/Event"));
    EventSymbol = EventSymbolFinder.Object;
}

UMapGridUI* UMapGridUI::Init() {
    Sectors->ClearChildren();    
    for (int col=0; col < UMapChunk::CHUNK_SIZE; col++) {        
        for (int row=0; row < UMapChunk::CHUNK_SIZE; row++) {
            const auto img = WidgetTree->ConstructWidget<UImage>();
            img->SetColorAndOpacity(FLinearColor(.027451, .031373, .254902));

            const auto slot = Sectors->AddChildToUniformGrid(img, row, col);
            slot->SetHorizontalAlignment(HAlign_Fill);
            slot->SetVerticalAlignment(VAlign_Fill);
        }
    }

    Tokens->ClearChildren();
    return this;
}

bool UMapGridUI::IsSelected(const UMapTokenUI* token) const {    
    if (SelectedScout != nullptr && SelectedScout == Cast<UScout>(token->Object))
        return true;
    return false;
}

void UMapGridUI::Deselect() {    
    if (SelectedScout) {
        GetToken(SelectedScout)->Highlight(false);
        SelectedScout = nullptr;

        for (const auto waypoint : Waypoints)
            waypoint->RemoveFromParent();
        Waypoints.Empty();
    }
}

void UMapGridUI::ClickedScout(UScout* scout) {
    if (scout == SelectedScout)
        return;

    Deselect();
    SelectedScout = scout;
    GetToken(scout)->Highlight(true);
    UpdateWaypoints(scout);
}

int32 UMapGridUI::NativePaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled
) const {
    for (const auto token : Tokens->GetAllChildren()) {
        if (const auto tokenUI = Cast<UMapTokenUI>(token)) {
            if (const auto scout = Cast<UScout>(tokenUI->Object))
                CastChecked<UCanvasPanelSlot>(tokenUI->Slot)->SetPosition(scout->Position * 50.);
        }
    }

    if (SelectedScout) {
        // draw a line with nodes for all targets of the scout
        SelectedScout->SanitizeTargets();
        TArray<FVector2d> linePoints;        
        linePoints.Add(SelectedScout->Position * 50.);
        for (const auto& target : SelectedScout->GetTargets())
            linePoints.Add(target.Position * 50.);
    
        if (linePoints.Num() > 1)
            FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), linePoints);
    }

    return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FReply UMapGridUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {    
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
        Deselect();

    if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
        if (SelectedScout) {        
            const auto pos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) / 50.;
            int column = pos.X;
            int row = pos.Y;

            
            if (The::Map(this)->CurrentChunk->IsRevealed(column, row)) {
                SelectedScout->AddTarget(pos, false);
            } else {
                if (!SelectedScout->RemoveTarget(column, row))
                    SelectedScout->AddTarget(FVector2D(column + 0.5, row + 0.5), true);
            }
        }
    }

    return FReply::Handled();
}

FReply UMapGridUI::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
    const auto pos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) / 50.;
    UX::GetParentWidget<UMapUI>(this)->UpdateMousePos(pos.X, pos.Y);

    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UMapGridUI::Clicked(UMapTokenUI* token) {
    if (const auto scout = Cast<UScout>(token->Object))
        ClickedScout(scout);
    if (const auto event = Cast<UEventInstance>(token->Object)) {
        Deselect();
        event->OpenUI();
    }
}

void UMapGridUI::RevealSector(int column, int row) const {
    for (const auto child : Sectors->GetSlots()) {
        const auto gridSlot = Cast<UUniformGridSlot>(child);
        if (gridSlot->GetRow() == row && gridSlot->GetColumn() == column) {
            child->Content->RemoveFromParent();
            if (row == UMapChunk::CHUNK_SIZE-1 && column == UMapChunk::CHUNK_SIZE-1)
                AddAnchor();
            return;
        }
    }
}

void UMapGridUI::AddScout(UScout* scout) {
    const auto scoutToken = CreateWidget<UMapTokenUI>(this, UMapTokenUIClass);
    scoutToken->Image->SetBrushFromTexture(ScoutSymbol);
    scoutToken->Object = scout;

    const auto slot = Tokens->AddChildToCanvas(scoutToken);
    slot->SetSize({25, 25});
    slot->SetAlignment({0.5, 0.5});
}
void UMapGridUI::AddEvent(UEventInstance* event) {    
    const auto eventToken = CreateWidget<UMapTokenUI>(this, UMapTokenUIClass);
    eventToken->Image->SetBrushFromTexture(EventSymbol);
    eventToken->Object = event;

    const auto slot = Tokens->AddChildToCanvas(eventToken);
    slot->SetSize({25, 25});
    slot->SetAlignment({0.5, 0.5});
    slot->SetPosition({
        event->Coordinate.Column * 50. + 25.,
        event->Coordinate.Row * 50. + 25.,
    });
}

void UMapGridUI::UpdateWaypoints(UScout* scout) {
    if (scout != SelectedScout)
        return;

    for (const auto waypoint : Waypoints)
        waypoint->RemoveFromParent();
    Waypoints.Empty();

    int i = 0;
    for (const auto& target : scout->GetTargets()) {
        const auto waypoint = UX::CreateToken(WidgetTree, FText::AsNumber(++i));
        const auto slot = Tokens->AddChildToCanvas(waypoint);

        slot->SetSize({25,25});
        slot->SetAlignment({0.5, 0.5});
        slot->SetPosition(target.Position * 50.);
        Waypoints.Add(waypoint);
    }
}
