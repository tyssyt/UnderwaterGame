// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Internationalization/Text.h"
#include "Engine/Texture2D.h"
#include "Resource.generated.h"

UCLASS()
class XD_API UResource : public UObject {
    GENERATED_BODY()

public:
    UResource();
    virtual ~UResource() override;
    UResource* Init(const FText& name, const TCHAR* image, const FText& description);

    UPROPERTY(VisibleAnywhere)
    FText Name;

    UPROPERTY(VisibleAnywhere)
    FText Description;

    UPROPERTY(VisibleAnywhere)
    UTexture2D* Image;
};
