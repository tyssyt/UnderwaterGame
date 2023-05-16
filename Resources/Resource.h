// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Internationalization/Text.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Resource.generated.h"

UCLASS()
class XD_API UResource : public UObject {
    GENERATED_BODY()

public:
    UResource();
    virtual ~UResource() override;
    UResource* Init(FText name, int relativeInventorySize, const TCHAR* image);


    UPROPERTY(VisibleAnywhere)
    FText Name;

    //const FText Description;


    UPROPERTY(VisibleAnywhere)
    UTexture2D* Image;

    UPROPERTY(VisibleAnywhere)
    int RelativeInventorySize;
};
