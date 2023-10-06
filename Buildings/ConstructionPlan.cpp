// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructionPlan.h"

Material::Material() : amount(0), resource(nullptr) {}
Material::Material(int amount, UResource* resource) : amount(amount), resource(resource) {}
Material::~Material() {}

Material* Material::Find(TArray<Material>& in, const UResource* resource) {
    for (auto& mat : in)
        if (mat.resource == resource)
            return &mat;
    return nullptr;
}

const Material* Material::Find(const TArray<Material>& in, const UResource* resource) {
    for (auto& mat : in)
        if (mat.resource == resource)
            return &mat;
    return nullptr;
}

void Material::AddTo(TArray<Material>& to, const Material& mat, int factor) {    
    if (const auto into = Find(to, mat.resource))
        into->amount += factor * mat.amount;
    else
        to.Emplace(factor * mat.amount, mat.resource);
}

void Material::AddTo(TArray<Material>& to, const TArray<Material>& from, int factor) {
    for (const auto& mat : from)
        AddTo(to, mat, factor);
}

UConstructionPlan* UConstructionPlan::Init(
    UClass* buildingClass,
    const TSubclassOf<UBuilderModeExtension> builderModeExtension,
    const FText& name,
    const TCHAR* image,
    const int time,
    const TArray<Material>& materials,
    const FText& description
) {
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Construction Plan, Name is blank: %s"), *name.ToString());
        return nullptr;
    }

    BuilderModeExtension = builderModeExtension;

    BuildingClass = buildingClass;
    Name = name;
    Time = time;
    
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load %s"), image);
        
    Materials.Append(materials);
    Description = description;
    return this;
}
