// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/Buildings/XActor.h"
#include "NaturalResource.generated.h"

class UEncyclopediaEntry;

UCLASS(Abstract)
class XD_API ANaturalResourceActor : public AXActor {
    GENERATED_BODY()
};

UCLASS()
class XD_API UNaturalResource : public UObject {
    GENERATED_BODY()

public:
    UNaturalResource* Init(UClass* buildingClass, const FText& name, const TCHAR* image, const FText& description);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UClass* BuildingClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTexture2D* Image;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UEncyclopediaEntry* EncyclopediaEntry;

    bool operator<(const UNaturalResource& other) const;
};
