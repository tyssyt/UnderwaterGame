// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentLoader.h"

#include "ComponentX.h"
#include "Buildings/Building.h"

UComponentInfo* UComponentInfo::Init(const FText& name, UClassInfo* classInfo) {
    Name = name;
    ClassInfo = classInfo;
    return this;
}

void UComponentInfo::AddNeed(UResource* resource, PropertyInfo* amountRef) {    
    Needs.Emplace(resource, amountRef);
}

UComponentLoader* UComponentLoader::Init(UComponentInfo* componentInfo) {
    ComponentInfo = componentInfo;
    return this;
}

UComponentLoader::~UComponentLoader() {
    for (const auto& val : PropertyValues)
        if (val.Key->Property->IsA<FObjectProperty>())
            static_cast<UObject*>(val.Value)->RemoveFromRoot();
        else
            val.Key->Property->DestroyValue(val.Value);
}

void UComponentLoader::SetProperty(PropertyInfo* property, void* value) {
    if (property->Property->IsA<FObjectProperty>())
        static_cast<UObject*>(value)->AddToRoot();
    PropertyValues.Add(property, value);
}

UActorComponent* UComponentLoader::AddComponentToBuilding(ABuilding* building) const {
    const auto component = NewObject<UActorComponent>(building, ComponentInfo->GetClassInfo()->GetBaseClass());
    if (const auto componentX = Cast<UComponentX>(component))
        componentX->Init(ComponentInfo);
    component->RegisterComponent(); // TODO figure out what this does and when it is needed
    // TODO when I add components that attach, I need to also do that I think
    // component->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    building->AddInstanceComponent(component); // TODO figure out what this does and when it is needed

    if (const auto sceneComponent = Cast<USceneComponent>(component)) {
        if (building->HasValidRootComponent())
            sceneComponent->AttachToComponent(building->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
        else
            building->SetRootComponent(sceneComponent);
    }

    for (const auto& val : PropertyValues)
        val.Key->SetValueIn(component, val.Value);
    return component;
}

TArray<Material> UComponentLoader::GetNeeds() {    
    TArray<Material> needs;
    for (const auto need : ComponentInfo->Needs)
        needs.Emplace(*GetValue<int>(need.AmountRef), need.Resource);
    return MoveTemp(needs);
}
