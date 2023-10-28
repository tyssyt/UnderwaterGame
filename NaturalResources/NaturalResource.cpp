// Fill out your copyright notice in the Description page of Project Settings.

#include "NaturalResource.h"

UNaturalResource* UNaturalResource::Init(UClass* buildingClass, const FText& name, const TCHAR* image, const FText& description) {
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Natural Resource, Name is blank: %s"), *name.ToString());
        return nullptr;
    }
    BuildingClass = buildingClass;
    Name = name;
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load Texture %s"), image);
    Description = description;
    return this;
}

bool UNaturalResource::operator<(const UNaturalResource& other) const {
    return Name.CompareTo(other.Name) < 0;
}
