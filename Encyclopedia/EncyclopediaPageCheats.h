// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EncyclopediaPage.h"
#include "Components/CheckBox.h"
#include "EncyclopediaPageCheats.generated.h"

class UEncyclopedia;

UCLASS(Abstract)
class XD_API UEncyclopediaPageCheats : public UEncyclopediaPage {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* InstantBuild;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* AlwaysPowered;

    UFUNCTION()
    void OnInstantBuildChanged(bool bIsChecked);
    UFUNCTION()
    void OnAlwaysPoweredChanged(bool bIsChecked);

public:
    UEncyclopediaPageCheats* Init();
};
