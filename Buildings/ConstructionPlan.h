// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/Resources/Resource.h"

#include <vector>

#include "ConstructionPlan.generated.h"

struct XD_API Material {
    Material();
    Material(int amount, UResource* resource);
    ~Material();

    int amount;
    UResource* resource;

    static Material* Find(std::vector<Material>& in, const UResource* resource);
    static const Material* Find(const std::vector<Material>& in, const UResource* resource);
    static void AddTo(std::vector<Material>& to, const Material& mat, int factor = 1);
    static void AddTo(std::vector<Material>& to, const std::vector<Material>& from, int factor = 1);
};

UCLASS()
class XD_API UConstructionPlan : public UObject {
    GENERATED_BODY()

public:
    UConstructionPlan* Init(UClass* buildingClass, FText name, const TCHAR* image, int time, std::initializer_list<Material> materials);

    UPROPERTY(VisibleAnywhere)
    UClass* BuildingClass;

    UPROPERTY(VisibleAnywhere)
    FText Name;

    //const FText Description;

    UPROPERTY(VisibleAnywhere)
    UTexture2D* Image;
    
    UPROPERTY(VisibleAnywhere)
    int Time;
    
    std::vector<Material> Materials;
};
