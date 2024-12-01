// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaPageEvent.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "XD/Map/Map.h"
#include "XD/Map/Event/EventUI.h"

UEncyclopediaPageEvent* UEncyclopediaPageEvent::Init(UEvent* Event) {
    Description->SetText(Event->GetStage(0)->Text);

    const auto font = Cast<UTextBlock>(Rewards->GetChildAt(0))->GetFont();

    Rewards->ClearChildren();
    for (const auto stage : Event->Stages) {
        for (const auto option : stage->Options) {
            // TODO instead, have a widget per reward and show that, and make a hbox per option if one option has multiple rewards
            if (option->Rewards.IsEmpty())
                continue;

            const auto textBlock = WidgetTree->ConstructWidget<UTextBlock>();
            textBlock->SetText(option->Text);
            textBlock->SetFont(font);
            Rewards->AddChildToVerticalBox(textBlock);

        }
    }

    LocationFont = Cast<UTextBlock>(Location->GetChildAt(0))->GetFont();
    Location->ClearChildren();
    for (const auto instance : Event->Instances)
        if (instance->Coordinate.IsRevealed())
            AddLocation(instance);

    return this;
}

void UEncyclopediaPageEvent::AddLocation(UEventInstance* EventInstance) const {    
    const auto textBlock = WidgetTree->ConstructWidget<UTextBlock>();
    textBlock->SetText(FText::FromString(UMap::CoordinateToString(EventInstance->Coordinate.Column, EventInstance->Coordinate.Row)));
    textBlock->SetFont(LocationFont);
    textBlock->SetColorAndOpacity(FSlateColor( FLinearColor( 0.f, 0.737255f, 1.f ) ));

    const auto button = WidgetTree->ConstructWidget<UButton>();
    button->AddChild(textBlock);
    button->SetBackgroundColor(FLinearColor(0.f,  0.f, 0.f, 0.f));
    button->OnClicked.AddDynamic(EventInstance, &UEventInstance::OpenUI);
    Location->AddChildToVerticalBox(button);
}
