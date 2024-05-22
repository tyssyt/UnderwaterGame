// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/Resources/Resource.h"
#include "ConstructionResources.generated.h"

class APickupPad;

struct XD_API FConstructionResource {
    const UResource* const Resource;
    int Total = 0;
    TArray<TPair<int, APickupPad*>> Pads;

    explicit FConstructionResource(const UResource* resource) : Resource(resource) {}
    TPair<int, APickupPad*>* FindNearestPad(const FVector& location, int amount);    
};

UCLASS()
class XD_API UConstructionResources : public UObject {
    GENERATED_BODY()

    TArray<FConstructionResource> ConstructionResources;

public:
    void SetResources(const TSet<UResource*>& constructionResources);

    FConstructionResource* Find(const UResource* resource);

    auto begin() { return ConstructionResources.begin(); }
    auto end() { return ConstructionResources.end(); }
    auto begin() const { return ConstructionResources.begin(); }
    auto end() const { return ConstructionResources.end(); }
};

inline auto begin(UConstructionResources* constructionResources) { return constructionResources->begin(); }
inline auto end(UConstructionResources* constructionResources) { return constructionResources->end(); }
inline auto begin(const UConstructionResources* constructionResources) { return constructionResources->begin(); }
inline auto end(const UConstructionResources* constructionResources) { return constructionResources->end(); }
