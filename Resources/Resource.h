// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Internationalization/Text.h"
#include "Engine/Texture2D.h"
#include "Resource.generated.h"

class UEncyclopediaEntry;

UCLASS()
class XD_API UResource : public UObject {
    GENERATED_BODY()

public:
    UResource* Init(const FText& name, const TCHAR* image, const FText& description);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTexture2D* Image;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UEncyclopediaEntry* EncyclopediaEntry;

    bool operator<(const UResource& other) const;
};
