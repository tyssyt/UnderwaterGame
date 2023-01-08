// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource.h"

UResource::UResource() {}
UResource::~UResource() {}

UResource* UResource::Init(FText name, int relativeInventorySize, const TCHAR* image) {
    Name = name;
    RelativeInventorySize = relativeInventorySize;
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load %s"), image);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Loaded %s to %d"), image, Image);
    }
    return this;
}
