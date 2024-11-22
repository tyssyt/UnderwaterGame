// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "MainUI.generated.h"

class UHotbarDock;

UCLASS(Abstract)
class XD_API UMainUI : public UUserWidget {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCanvasPanel* Canvas;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHotbarDock* HotbarDock;
};