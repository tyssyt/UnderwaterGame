// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructionPlan.h"

Material::Material() : amount(0), resource(nullptr) {}
Material::Material(int amount, UResource* resource) : amount(amount), resource(resource) {}
Material::~Material() {}

Material* Material::Find(std::vector<Material>& in, const UResource* resource) {
    for (auto& mat : in)
        if (mat.resource == resource)
            return &mat;
    return nullptr;
}

const Material* Material::Find(const std::vector<Material>& in, const UResource* resource) {
    for (auto& mat : in)
        if (mat.resource == resource)
            return &mat;
    return nullptr;
}

void Material::AddTo(std::vector<Material>& to, const Material& mat, int factor) {    
    if (const auto into = Find(to, mat.resource))
        into->amount += factor * mat.amount;
    else
        to.emplace_back(factor * mat.amount, mat.resource);
}

void Material::AddTo(std::vector<Material>& to, const std::vector<Material>& from, int factor) {
    for (const auto& mat : from)
        AddTo(to, mat, factor);
}

UConstructionPlan* UConstructionPlan::Init(UClass* buildingClass, FText name, const TCHAR* image, int time, std::initializer_list<Material> materials) {
    BuildingClass = buildingClass;
    Name = name;
    Time = time;
    
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load %s"), image);
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Loaded %s to %d"), image, Image);
    }
        
    // TODO there is propably some sane way to do this, but it is beyond me... c++ is gonna c++
    for (auto& mat : materials) {
        Materials.push_back(mat);
    }
    
    return this;
}
