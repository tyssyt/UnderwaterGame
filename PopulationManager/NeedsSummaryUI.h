// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Need.h"
#include "NeedUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "NeedsSummaryUI.generated.h"


class UHabitatPopulationManager;

UCLASS(Abstract)
class XD_API UNeedsSummaryUI : public UUserWidget {
    GENERATED_BODY()

protected:
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UWrapBox* Needs;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UNeedUI> NeedUIClass;

    bool NeedsMatch(int population);
    void InitNeeds(int population);
    void UpdateSatisfied(const TArray<UNeed*>& satisfiedNeeds) const;    

public:
    void SetNeeds(UHabitatPopulationManager* populationManager);
};
