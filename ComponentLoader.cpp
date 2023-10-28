// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentLoader.h"

#include "ComponentX.h"
#include "Buildings/Building.h"

UComponentInfo* UComponentInfo::Init(const FText& name, const TSubclassOf<UActorComponent> componentClass) {
    Name = name;
    ComponentClass = componentClass;
    return this;
}

UComponentInfo::AddPropertyResult UComponentInfo::AddProperty(const FString& name, bool required) {    
    if (Properties.Contains(name))
        return AddPropertyResult::DuplicateProperty;
    
    for (TFieldIterator<FProperty> it(ComponentClass, EFieldIterationFlags::None); it; ++it) {
        const auto rawProp = *it;
        if (!rawProp->GetName().Equals(name))
            continue;

        Properties.Emplace(name, {rawProp, required});
        return AddPropertyResult::Success;
    }
    
    return AddPropertyResult::UnknownProperty;
}

void UComponentInfo::AddNeed(UResource* resource, FIntProperty* amountRef) {    
    Needs.Emplace(resource, amountRef);
}

UComponentLoader* UComponentLoader::Init(UComponentInfo* componentInfo) {
    ComponentInfo = componentInfo;
    return this;
}

UComponentLoader::~UComponentLoader() {
    for (const auto& val : PropertyValues)
        val.Key->DestroyValue(val.Value);
}

void UComponentLoader::SetProperty(FProperty* property, void* value) {
    PropertyValues.Add(property, value);
}

UActorComponent* UComponentLoader::AddComponentToBuilding(ABuilding* building) const {
    const auto component = NewObject<UComponentX>(building, ComponentInfo->ComponentClass)->Init(ComponentInfo);
    component->RegisterComponent(); // TODO figure out what this does and when it is needed
    // TODO when I add components that attach, I need to also do that I think
    // component->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
    building->AddInstanceComponent(component); // TODO figure out what this does and when it is needed

    for (const auto& val : PropertyValues) {
        const auto prop = val.Key;
        prop->CopySingleValue(prop->ContainerPtrToValuePtr<void>(component), val.Value);
    }

    return component;
}

TArray<Material> UComponentLoader::GetNeeds() {    
    TArray<Material> needs;
    for (const auto need : ComponentInfo->Needs)
        needs.Emplace(*GetValue<int>(need.AmountRef), need.Resource);
    return MoveTemp(needs);
}
