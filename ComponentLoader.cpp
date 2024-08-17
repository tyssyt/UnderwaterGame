// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentLoader.h"

#include "ComponentX.h"
#include "Buildings/Building.h"

UComponentInfo::~UComponentInfo() {
    for (const auto& property : Properties)
        if (property.Value.DefaultValue && property.Value.Property->IsA<FObjectProperty>())
            static_cast<UObject*>(property.Value.DefaultValue)->RemoveFromRoot();
}

UComponentInfo* UComponentInfo::Init(const FText& name, const TSubclassOf<UActorComponent> componentClass) {
    Name = name;
    ComponentClass = componentClass;
    return this;
}

void UComponentInfo::AddProperty(const FString& name, UFunction* setter, FProperty* prop, bool required, void* defaultValue) {    
    if (defaultValue && prop->IsA<FObjectProperty>())
        static_cast<UObject*>(defaultValue)->AddToRoot();

    Properties.Emplace(name, {setter, prop, required, defaultValue});
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

void CopyValue(FProperty* prop, void* container, void* val) {
    if (prop->IsA<FObjectProperty>())
        prop->CopySingleValue(prop->ContainerPtrToValuePtr<void>(container), &val);
    else
        prop->CopySingleValue(prop->ContainerPtrToValuePtr<void>(container), val);
}

void CallSingleParamFunction(UFunction* function, UObject* self, FProperty* param, void* arg) {
    uint8* params = static_cast<uint8*>(FMemory_Alloca(function->ParmsSize));
    FMemory::Memzero(params, function->ParmsSize);
    CopyValue(param, params, arg);

    self->ProcessEvent(function, params);
}

UActorComponent* UComponentLoader::AddComponentToBuilding(ABuilding* building) const {
    const auto component = NewObject<UActorComponent>(building, ComponentInfo->ComponentClass);
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

    for (const auto& val : PropertyValues) {
        if (const auto function = val.Key->Function)
            CallSingleParamFunction(function, component, val.Key->Property, val.Value);
        else if (const auto prop = val.Key->Property)
            CopyValue(prop, component, val.Value);
    }
    return component;
}

TArray<Material> UComponentLoader::GetNeeds() {    
    TArray<Material> needs;
    for (const auto need : ComponentInfo->Needs)
        needs.Emplace(*GetValue<int>(need.AmountRef), need.Resource);
    return MoveTemp(needs);
}
