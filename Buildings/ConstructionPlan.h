// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include <vector>

#include "ConstructionPlan.generated.h"

struct XD_API Material {
    Material();
    Material(int amount, UResource* resource);
    ~Material();

    int amount;
    UResource* resource;
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
