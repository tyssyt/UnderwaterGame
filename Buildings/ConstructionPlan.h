// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/Resources/Resource.h"

#include "ConstructionPlan.generated.h"

class UBuilderModeExtension;

struct XD_API Material {
    Material();
    Material(int amount, UResource* resource);
    ~Material();

    int amount;
    UResource* resource;

    static Material* Find(TArray<Material>& in, const UResource* resource);
    static const Material* Find(const TArray<Material>& in, const UResource* resource);
    static void AddTo(TArray<Material>& to, const Material& mat, int factor = 1);
    static void AddTo(TArray<Material>& to, const TArray<Material>& from, int factor = 1);
};

UCLASS()
class XD_API UConstructionPlan : public UObject {
    GENERATED_BODY()

public:
    UConstructionPlan* Init(
        UClass* buildingClass,
        const TSubclassOf<UBuilderModeExtension> builderModeExtension,
        const FText& name,
        const TCHAR* image,
        const int time,
        const TArray<Material>& materials,
        const FText& description
    );

    UPROPERTY(VisibleAnywhere)
    UClass* BuildingClass;

    UPROPERTY(VisibleAnywhere)
    TSubclassOf<UBuilderModeExtension> BuilderModeExtension;

    UPROPERTY(VisibleAnywhere)
    FText Name;

    UPROPERTY(VisibleAnywhere)
    UTexture2D* Image;

    UPROPERTY(VisibleAnywhere)
    int Time;

    TArray<Material> Materials;    

    UPROPERTY(VisibleAnywhere)
    FText Description;
};
