// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/ComponentLoader.h"
#include "XD/NaturalResources/NaturalResource.h"
#include "XD/Resources/Resource.h"

#include "ConstructionPlan.generated.h"

class UEncyclopediaEntry;
class UBuilderModeExtension;

struct XD_API Material {
    Material();
    Material(int amount, UResource* resource);

    int amount;
    UResource* resource;

    static Material* Find(TArray<Material>& in, const UResource* resource);
    static const Material* Find(const TArray<Material>& in, const UResource* resource);
    static void AddTo(TArray<Material>& to, const Material& mat, int factor = 1);
    static void AddTo(TArray<Material>& to, const TArray<Material>& from, int factor = 1);
    static void RemoveFrom(TArray<Material>& to, const Material& mat);
};

UCLASS()
class XD_API UConstructionPlan : public UObject {
    GENERATED_BODY()

public:
    UConstructionPlan* Init(
        UClass* buildingClass,
        const FText& name,
        const TCHAR* image,
        const TArray<UComponentLoader*>& componentLoaders,
        const int time,
        UNaturalResource* constructedOn,
        const TArray<Material>& materials,
        const FText& category,
        const FText& description
    );

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UClass* BuildingClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UTexture2D* Image;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<UComponentLoader*> ComponentLoaders;    

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int Time;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UNaturalResource* ConstructedOn;

    TArray<Material> Materials;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Category;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UEncyclopediaEntry* EncyclopediaEntry;

    TArray<Material> GetNeeds() const;

    bool operator<(const UConstructionPlan& other) const;
    static bool CompareByComplexity(const UConstructionPlan& one, const UConstructionPlan& two);
};
