// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionResources.h"

#include "XD/Buildings/PickupPad.h"

TPair<int, APickupPad*>* FConstructionResource::FindNearestPad(const FVector& location, int amount) {
    TPair<int, APickupPad*>* nearest = nullptr;
    double nearestDist = INFINITY;

    for (auto& pad : Pads) {
        if (pad.Key < amount)
            break;
        const double dist = FVector::Distance(location, pad.Value->GetActorLocation());
        if (dist < nearestDist) {
            nearest = &pad;
            nearestDist = dist;
        }
    }
    return nearest;
}

void UConstructionResources::SetResources(const TSet<UResource*>& constructionResources) {
    for (const auto resource : constructionResources)
        ConstructionResources.Emplace(resource);
}

FConstructionResource* UConstructionResources::Find(const UResource* resource) {    
    for (auto& res : ConstructionResources)
        if (res.Resource == resource)
            return &res;
    return nullptr;
}
