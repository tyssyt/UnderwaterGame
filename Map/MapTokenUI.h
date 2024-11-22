// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MapTokenUI.generated.h"

UCLASS(Abstract)
class XD_API UMapTokenUI : public UUserWidget {
    GENERATED_BODY()

protected:    
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;
    
    UPROPERTY()
    UObject* Object;

    void Highlight(bool highlighted);
};
