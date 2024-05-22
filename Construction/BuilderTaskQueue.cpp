// Fill out your copyright notice in the Description page of Project Settings.

#include "BuilderTaskQueue.h"

bool CheckMaterial(const Material& material, UConstructionResources* constructionResources) {    
    const auto constructionResource = constructionResources->Find(material.resource);
    return !constructionResource->Pads.IsEmpty() && constructionResource->Pads[0].Key >= material.amount;
}

UBuilderTask* UBuilderTaskQueue::TryDequeueTask(UConstructionResources* constructionResources) {
    UBuilderTask* task;
    if (!!Tasks.Dequeue(task) || !task->RequiredMaterial.resource || CheckMaterial(task->RequiredMaterial, constructionResources))
        return task;

    AddTaskWithMissingResource(task);
    return nullptr;
}

void UBuilderTaskQueue::AddTaskWithMissingResource(UBuilderTask* task) {
    check(task->RequiredMaterial.resource);
    for (auto& tasks : TasksWithMissingResources) {
        if (tasks.Resource == task->RequiredMaterial.resource) {
            tasks.Queue.Enqueue(task);
            return;
        }
    }

    const int i = TasksWithMissingResources.Emplace(task->RequiredMaterial.resource);
    TasksWithMissingResources[i].Queue.Enqueue(task);
}

UBuilderTask* UBuilderTaskQueue::Dequeue(UConstructionResources* constructionResources) {
    for (auto& tasks : TasksWithMissingResources) {
        const auto task = *tasks.Queue.Peek();

        if (CheckMaterial(task->RequiredMaterial, constructionResources)) {
            tasks.Queue.Pop();
            if (tasks.Queue.IsEmpty())
                TasksWithMissingResources.RemoveAtSwap(&tasks - TasksWithMissingResources.GetData());
            return task;
        }
    }

    while (!Tasks.IsEmpty()) {
        if (const auto task = TryDequeueTask(constructionResources))
            return task;
    }

    return nullptr;
}
