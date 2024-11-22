// Fill out your copyright notice in the Description page of Project Settings.

#include "EventUI.h"

#include "The.h"
#include "UI.h"
#include "Components/CanvasPanelSlot.h"
#include "XD/BlueprintHolder.h"
#include "XD/Map/MapGridUI.h"

void UEventOptionUI::OnClick() {
    Button->OnClicked.Clear();

    if (const auto eventUI = UX::GetParentWidget<UEventUI>(this)) {
        // update event ui with new stage
        if (const auto stage = EventOption->Continuation)
            eventUI->AddStage(stage, nullptr);

        // register Option on event obj
        const auto event = eventUI->GetEvent();
        eventUI->GetEvent()->ChosenOptions.Add(EventOption);

        for (const auto reward : EventOption->Rewards)
            reward->Give(event);
    }

    // remove other options and disable click
    if (const auto stageUI = UX::GetParentWidget<UEventStageUI>(this))
        stageUI->Chose(EventOption);
}

UEventOptionUI* UEventOptionUI::Init(UEventOption* eventOption, bool enabled) {
    EventOption = eventOption;

    Text->SetText(eventOption->Text);
    Button->SetIsEnabled(enabled);

    if (enabled) {
        Button->OnClicked.AddDynamic(this, &UEventOptionUI::OnClick);
    }

    return this;
}

UEventStageUI* UEventStageUI::Init(UEventStage* eventStage, UEventOption* chosenOption) {
    EventStage = eventStage;

    Text->SetText(eventStage->Text);
    if (chosenOption) {
        Chose(chosenOption);
    } else {
        Options->ClearChildren();
        for (const auto option : eventStage->Options) {
            const auto optionWidget = CreateWidget<UEventOptionUI>(GetOwningPlayer(), UEventOptionUIClass)->Init(option, true);
            Options->AddChildToWrapBox(optionWidget);
        }
    }
    return this;
}
void UEventStageUI::Chose(UEventOption* option) const {    
    Options->ClearChildren();
    const auto optionWidget = CreateWidget<UEventOptionUI>(GetOwningPlayer(), UEventOptionUIClass)->Init(option, false);
    Options->AddChildToWrapBox(optionWidget);
}

UEventUI* UEventUI::Init(UEventInstance* event) {
    Event = event;

    EventName->SetText(event->Event->Name);
    Background->SetBrushFromTexture(event->Event->Image);
    Stages->ClearChildren();

    auto nextStage = event->Event->GetStage(0);
    for (const auto option : event->ChosenOptions) {
        check(nextStage);
        AddStage(nextStage, option);
        nextStage = option->Continuation;
    }
    if (nextStage)
        AddStage(nextStage, nullptr);

    return this;
}

void UEventUI::AddStage(UEventStage* eventStage, UEventOption* chosenOption) const {    
    const auto stageWidget = CreateWidget<UEventStageUI>(GetOwningPlayer(), UEventStageUIClass)->Init(eventStage, chosenOption);
    Stages->AddChild(stageWidget);
}

void UEventUI::Open() {    
    const auto slot = The::BPHolder(this)->MainUI->Canvas->AddChildToCanvas(this);
    slot->SetSize({1200,900});
    slot->SetAnchors(FAnchors(0.5));
    slot->SetAlignment({0.5, 0.5});
    GetOwningPlayer()->InputComponent->BindAction("Deselect", IE_Pressed, this, &UEventUI::Close);
}
void UEventUI::Close() {    
    RemoveFromParent();
    GetOwningPlayer()->InputComponent->RemoveActionBinding("Deselect", IE_Pressed);
}
