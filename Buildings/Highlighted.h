// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Highlighted.generated.h"

UCLASS()
class XD_API UHighlighted : public UCondition {
    GENERATED_BODY()
public:
    static FLinearColor Blue;
    static FLinearColor Gray;
    static FLinearColor Green;
    static FLinearColor Red;
    static FLinearColor Yellow;

    UHighlighted();
    UHighlighted* SetColor(FLinearColor color);
};
