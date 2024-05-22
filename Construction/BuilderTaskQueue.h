// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuilderShip.h"
#include "ConstructionResources.h"
#include "UObject/Object.h"
#include "BuilderTaskQueue.generated.h"

UCLASS()
class XD_API UBuilderTaskQueue : public UObject {
    GENERATED_BODY()

    struct FMissingResourceTasks {
        explicit FMissingResourceTasks(UResource* resource) : Resource(resource) {}
        UResource* Resource;
        TQueue<UBuilderTask*> Queue;
    };

    TQueue<UBuilderTask*> Tasks;
    TArray<FMissingResourceTasks> TasksWithMissingResources;

    UBuilderTask* TryDequeueTask(UConstructionResources* constructionResources);
    void AddTaskWithMissingResource(UBuilderTask* task);

public:
    void Enqueue(UBuilderTask* task) { Tasks.Enqueue(task); }
    UBuilderTask* Dequeue(UConstructionResources* constructionResources);
};
