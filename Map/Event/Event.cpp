// Fill out your copyright notice in the Description page of Project Settings.

#include "Event.h"

#include "EventUI.h"
#include "The.h"
#include "../MapChunk.h"
#include "Blueprint/UserWidget.h"
#include "XD/BlueprintHolder.h"

UEvent* UEvent::Init(const FText& name, const TCHAR* image) {    
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Event, Name is blank: %s"), *name.ToString());
        return nullptr;
    }
    Name = name;
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load Texture %s"), image);
    return this;
}

void UEvent::AddStage(const FText& text) {
    const auto stage = NewObject<UEventStage>(this);
    stage->Text = text;
    Stages.Add(stage);
}

UEventInstance* UEvent::CreateInstance(const FMapCoordinate& coordinate) {
    const auto instance = NewObject<UEventInstance>(this)->Init(this, coordinate);
    Instances.Add(instance);
    coordinate.Chunk->AddEvent(instance);
    return instance;
}

void UEventStage::AddOption(const FText& text, const TArray<UReward*>& rewards, UEventStage* continuation) {
    const auto option = NewObject<UEventOption>(this);
    option->Text = text;
    option->Rewards.Append(rewards);
    option->Continuation = continuation;
    Options.Add(option);
}

UEventInstance* UEventInstance::Init(UEvent* event, const FMapCoordinate& coordinate) {
    Event = event;
    Coordinate = coordinate;
    return this;
}

void UEventInstance::OpenUI() {
    CreateWidget<UEventUI>(The::PlayerController(this), The::BPHolder(this)->UEventUIClass)->Init(this)->Open();
}
