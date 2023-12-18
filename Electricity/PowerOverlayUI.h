// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CheckBox.h"
#include "PowerOverlayUI.generated.h"

enum class EPowerOverlayMode;

UCLASS(Abstract)
class XD_API UPowerOverlayUI : public UUserWidget {
    GENERATED_BODY()
  
protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* CheckBoxTogglePower;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* CheckBoxConnect;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* CheckBoxDisconnect;

public:

    EPowerOverlayMode GetMode() const;
    void Reset() const;
};

