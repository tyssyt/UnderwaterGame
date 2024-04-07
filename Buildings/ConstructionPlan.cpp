// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionPlan.h"

Material::Material() : amount(0), resource(nullptr) {}
Material::Material(int amount, UResource* resource) : amount(amount), resource(resource) {}

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
    const FText& name,
    const TCHAR* image,
    const TArray<UComponentLoader*>& componentLoaders,
    const int time,
    UNaturalResource* constructedOn,
    const TArray<Material>& materials,
    const FText& category,
    const FText& description
) {
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Construction Plan, Name is blank: %s"), *name.ToString());
        return nullptr;
    }

    BuildingClass = buildingClass;
    Name = name;
    Time = time;

    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load %s"), image);

    ComponentLoaders = componentLoaders;
    ConstructedOn = constructedOn;
    Materials = materials ;
    Category = category;
    Description = description;
    return this;
}

TArray<Material> UConstructionPlan::GetNeeds() const { 
    TArray<Material> needs;
    for (const auto component : ComponentLoaders)
        needs.Append(component->GetNeeds());
    return MoveTemp(needs);
}

bool UConstructionPlan::operator<(const UConstructionPlan& other) const {
    return Name.CompareTo(other.Name) < 0;
}

bool UConstructionPlan::CompareByComplexity(const UConstructionPlan& one, const UConstructionPlan& two) {
    // no ConstructedOn is smaller
    const int c1 = (!!one.ConstructedOn) - (!!two.ConstructedOn);    
    if (c1 != 0)
        return c1 < 0;

    // then number of materials
    const int c2 = one.Materials.Num() - two.Materials.Num();
    if (c2 != 0)
        return c2 < 0;

    // then name
    const int c3 = one.Name.CompareTo(two.Name);
    if (c3 != 0)
        return c2 < 0;

    checkNoEntry();
    return &one - &two < 0;
}
