// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapGridUI.h"
#include "Blueprint/UserWidget.h"
#include "MapUI.generated.h"

UCLASS(Abstract)
class XD_API UMapUI : public UUserWidget {
    GENERATED_BODY()
    
protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UMapGridUI* MapGrid;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Subscript;

public:
    UMapUI* Init();

    UFUNCTION(BlueprintCallable)
    void Open();
    UFUNCTION(BlueprintCallable)
    void Close();
    
    void RevealSector(int column, int row) const;

    void AddScout(UScout* scout);
    void AddEvent(UEventInstance* event);

    void UpdateWaypoints(UScout* scout);

    void UpdateMousePos(int column, int row) const;
};
