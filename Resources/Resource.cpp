// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource.h"

UResource::UResource() {}
UResource::~UResource() {}

UResource* UResource::Init(const FText& name, const TCHAR* image, const FText& description) {
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Resource, Name is blank: %s"), *name.ToString());
        return nullptr;
    }
    Name = name;
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load Texture %s"), image);
    Description = description;
    return this;
}
