// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceRatioUI.h"


UResourceRatioUI* UResourceRatioUI::Init(
    int numeratorAmount,
    UResource* numeratorResource,
    int denominatorAmount,
    UResource* denominatorResource
) {
    Numerator->Init(numeratorAmount, numeratorResource);
    Denominator->Init(denominatorAmount, denominatorResource);
    return this;
}
