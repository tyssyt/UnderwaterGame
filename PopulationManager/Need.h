// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Need.generated.h"

class AHabitat;
class UEncyclopediaEntry;

UCLASS()
class XD_API UNeed : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Name;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTexture2D* Image;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int NotificationPop;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int StartPop;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UEncyclopediaEntry* EncyclopediaEntry;

    UNeed* Init(const FText& name, const TCHAR* image, const FText& description, const int notificationPop, const int startPop);

    int IsSatisfied(const AHabitat* habitat, int population);

    bool operator<(const UNeed& other) const;
};
