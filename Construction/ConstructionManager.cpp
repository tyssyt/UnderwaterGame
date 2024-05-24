// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionManager.h"

#include "Collections.h"

UConstructionManager::UConstructionManager() {
    Tasks = CreateDefaultSubobject<UBuilderTaskQueue>(TEXT("Tasks"));
    ConstructionResources = CreateDefaultSubobject<UConstructionResources>(TEXT("ConstructionResources"));
}

APickupPad* UConstructionManager::GetNearestPickupPad(const FVector& location) {
    return Actors::FindNearest(location, PickupPads);;
}

void UConstructionManager::Tick(float DeltaTime) {
    // TODO if this never explodes, we can remove it at some point
    if (LastFrameNumberWeTicked == GFrameCounter) {
        checkNoEntry();
        return;
    }
    LastFrameNumberWeTicked = GFrameCounter;

    // collect which resources are where
    for (auto& constructionResource : ConstructionResources) {
        constructionResource.Total = 0;
        constructionResource.Pads.Empty();
        
        for (const auto pad : PickupPads) {
            const int resource = pad->GetUnreserved(constructionResource.Resource);
            if (resource > 0) {
                constructionResource.Total += resource;
                constructionResource.Pads.Emplace(resource, pad);
            }
        }

        constructionResource.Pads.Sort();
    }

    while (!IdleBuilders.IsEmpty()) {
        const auto task = Tasks->Dequeue(ConstructionResources);
        if (!task)
            break;

        FVector location = task->Location;
        if (task->RequiredMaterial.resource) {
            const auto constructionResource = ConstructionResources->Find(task->RequiredMaterial.resource);
            const auto pad = constructionResource->FindNearestPad(location, task->RequiredMaterial.amount);
            pad->Key -= task->RequiredMaterial.amount;
            pad->Value->Reserve(task->RequiredMaterial);
            task->PickupFrom = pad->Value;
            location = pad->Value->GetActorLocation();
        }

        const auto builder = Actors::FindNearest(location, IdleBuilders);
        IdleBuilders.Remove(builder);
        builder->DoTask(task);
    }
}
