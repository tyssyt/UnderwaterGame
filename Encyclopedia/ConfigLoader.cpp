// Fill out your copyright notice in the Description page of Project Settings.

#include "ConfigLoader.h"

#include "Node.h"
#include "Parsing.h"
#include "XD/PlayerControllerX.h"
#include "XD/Construction/BuildingBuilderMode.h"
#include "XD/Construction/ConveyorBuilderMode.h"
#include "XD/Construction/DismantleMode.h"
#include "XD/Construction/IndoorBuilderMode.h"
#include "XD/Hotbar/HotbarSlotAction.h"
#include "XD/Hotbar/HotbarSlotSubmenu.h"
#include "XD/PopulationManager/Need.h"
#include "XD/PopulationManager/NeedSatisfier.h"

// TODO use one error mechanism like a result class or exceptions and use it everywhere in here instead of mix & match
DEFINE_LOG_CATEGORY(LogConfigLoader);

struct Config {
    UObject* ObjOwner;
    APlayerControllerX* UIOwner;
    const TSubclassOf<UHotbar> HotbarUIClass;
    const TSubclassOf<UHotbarSlotSubmenu> HotbarSlotSubmenuUIClass;
    const TSubclassOf<UHotbarSlotAction> HotbarSlotActionUIClass;

    // TODO maybe I want FName instead of FString here
    TMap<FString, UComponentInfo*> Components;
    TMap<FString, UResource*> Resources;
    TMap<FString, UNaturalResource*> NaturalResources;
    TMap<FString, UConstructionPlan*> Buildings;
    TArray<URecipe*> Recipes;
    TMap<FString, UHotbar*> Hotbars;
    UHotbar* MainHotbar;
    TArray<TPair<FText, FText>> EncyclopediaPages;
    TMap<FString, UNeed*> Needs;
    TArray<UNeedSatisfier*> NeedSatisfiers;

    Config(
        UObject* ObjOwner,
        APlayerControllerX* UIOwner,
        const TSubclassOf<UHotbar>& HotbarUIClass,
        const TSubclassOf<UHotbarSlotSubmenu>& HotbarSlotSubmenuUIClass,
        const TSubclassOf<UHotbarSlotAction>& HotbarSlotActionUIClass
    )
        : ObjOwner(ObjOwner),
          UIOwner(UIOwner),
          HotbarUIClass(HotbarUIClass),
          HotbarSlotSubmenuUIClass(HotbarSlotSubmenuUIClass),
          HotbarSlotActionUIClass(HotbarSlotActionUIClass),
          MainHotbar(nullptr) {}
};

// TODO feels like there should be a better way to do this with unreal reflection, but I can't think of one
void* LoadFPropertyValue(const FYamlNode& node, FProperty* prop) {
    if (const auto _ = ExactCastField<FBoolProperty>(prop))
        return new bool(node.As<bool>());
    if (const auto _ = ExactCastField<FInt8Property>(prop))
        return new int8(node.As<int8>());
    if (const auto _ = ExactCastField<FInt16Property>(prop))
        return new int16(node.As<int16>());
    if (const auto _ = ExactCastField<FIntProperty>(prop))
        return new int32(node.As<int32>());
    if (const auto _ = ExactCastField<FInt64Property>(prop))
        return new int64(node.As<int64>());
    if (const auto _ = ExactCastField<FByteProperty>(prop))
        return new uint8(node.As<uint8>());
    if (const auto _ = ExactCastField<FUInt16Property>(prop))
        return new uint16(node.As<uint16>());
    if (const auto _ = ExactCastField<FUInt32Property>(prop))
        return new uint32(node.As<uint32>());
    if (const auto _ = ExactCastField<FUInt64Property>(prop))
        return new uint64(node.As<uint64>());
    if (const auto _ = ExactCastField<FFloatProperty>(prop))
        return new float(node.As<float>());
    if (const auto _ = ExactCastField<FDoubleProperty>(prop))
        return new double(node.As<double>());
    if (const auto objProperty = CastField<FObjectProperty>(prop)) {
        const auto obj = StaticLoadObject(objProperty->PropertyClass, nullptr, *node.As<FString>());
        return obj;        
    }
    if (const auto structProperty = CastField<FStructProperty>(prop)) {
        if (structProperty->Struct == TBaseStructure<FVector>::Get())
            return new FVector(node.As<FVector>());
        if (structProperty->Struct == TBaseStructure<FRotator>::Get())
            return new FRotator(node.As<FRotator>());
        if (structProperty->Struct == TBaseStructure<FColor>::Get())
            return new FColor(node.As<FColor>());
        if (structProperty->Struct == TBaseStructure<FLinearColor>::Get())
            return new FLinearColor(node.As<FLinearColor>());

        UE_LOG(LogConfigLoader, Error, TEXT("Cannot handle StructProperty %s with struct %s in line %d"), *structProperty->GetName(), *structProperty->Struct->GetName(), node.GetMark().line +1)
        checkNoEntry();
        return nullptr;
    }

    // TODO FTextProperty, FNameProperty, FStrProperty
    // TODO array, map, set, what about pair?

    UE_LOG(LogConfigLoader, Error, TEXT("Cannot handle Type of Property %s of class %s in line %d"), *prop->GetName(), *prop->GetClass()->GetName(), node.GetMark().line +1)
    checkNoEntry();
    return nullptr;
}

template<class T>
T* LoadReference(const FYamlNode& node, TMap<FString, T*>& map) {
    const FString name = node.As<FString>();
    if (const auto reference = map.Find(name))
        return *reference;
    UE_LOG(LogConfigLoader, Error, TEXT("Unable to find %s %s referenced on line %d"), *T::StaticClass()->GetName(), *name, node.GetMark().line +1);
    return nullptr;
}
template<class T>
T* LoadReferenceOpt(const FYamlNode& node, TMap<FString, T*>& map) {
    return node ? LoadReference(node, map) : nullptr;
}
template<class T>
TArray<T*> LoadReferences(const FYamlNode& node, TMap<FString, T*>& map) {
    TArray<T*> references;
    for (const auto& referenceNode : node)
        if (auto reference = LoadReference(referenceNode, map))
            references.Add(reference);
    return MoveTemp(references);
}

UClass* LoadClass(const FYamlNode& node) {
    UClass* uClass = FindFirstObjectSafe<UClass>(*node.As<FString>(), EFindFirstObjectOptions::EnsureIfAmbiguous, ELogVerbosity::Warning);
    if (!uClass)
        UE_LOG(LogConfigLoader, Error, TEXT("Failed to load UClass: %s in Block: %s, which starts on line %d"), *node["Class"].As<FString>(), *node.GetContent(), node.GetMark().line +1);
    return uClass;
}
UClass* LoadClass(const FYamlNode& node, UClass* defaultValue) {
    return node ? LoadClass(node) : defaultValue;
}

template<class T>
TOptional<T> LoadMaterial(const FYamlNode& node, Config& config) {
    try {
        if (const auto resource = LoadReference(node["Resource"], config.Resources))
            return TOptional<T>(InPlace, node["Amount"].As<int>(), resource);
        return {};
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Material: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
        return {};
    }
}
template<class T>
TArray<T> LoadMaterials(const FYamlNode& node, Config& config) {
    TArray<T> materials;
    for (const auto& materialNode : node)
        if (auto material = LoadMaterial<T>(materialNode, config))
            materials.Add(*material);
    return MoveTemp(materials);
}

void LoadNeed(const FYamlNode& node, UComponentInfo* componentInfo, Config& config) {    
    try {
        const auto resource = LoadReference(node["Resource"], config.Resources);
        if (!resource)
            return;

        const auto prop = componentInfo->Properties.Find(node["AmountRef"].As<FString>());
        if (!prop) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Component Need: %s is not a Property in Block: %s, which starts on line %d"), *node["AmountRef"].As<FString>(), *node.GetContent(), node.GetMark().line +1);
            return;
        }

        const auto intProp = CastField<FIntProperty>(prop->Property);
        if (!intProp) {
            UE_LOG(LogConfigLoader, Error, TEXT("Error loading Component Need: %s is not an IntProperty in Block: %s, which starts on line %d"), *prop->Property->GetName(), *node.GetContent(), node.GetMark().line +1);
            return;
        }

        componentInfo->AddNeed(resource, prop);
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Component Need: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}
TPair<UFunction*, FProperty*> FindProperty(const FString& name, const TSubclassOf<UActorComponent> componentClass) {
    for (TFieldIterator<UFunction> it(componentClass, EFieldIterationFlags::IncludeSuper); it; ++it) {
        const auto rawFunc = *it;
        if (!rawFunc->GetName().Equals(TEXT("Set") + name))
            continue;
        
        FProperty* param = nullptr;
        for (TFieldIterator<FProperty> paramIt(rawFunc); paramIt; ++paramIt) {
            if (!(paramIt->PropertyFlags & CPF_ReturnParm) && paramIt->PropertyFlags & CPF_Parm) {
                if (param) {
                    param = nullptr;
                    break;
                }
                param = *paramIt;
            }
        }
        if (!param)
            continue;

        return MakeTuple(rawFunc, param);
    }

    for (TFieldIterator<FProperty> it(componentClass, EFieldIterationFlags::IncludeSuper); it; ++it) {
        const auto rawProp = *it;
        if (!rawProp->GetName().Equals(name))
            continue;

        return MakeTuple(nullptr, rawProp);
    }
    return MakeTuple(nullptr, nullptr);
}
void LoadProperty(const FYamlNode& node, UComponentInfo* component) {    
    const auto propertyName = node["Name"].As<FString>();

    const auto [setter, prop] = FindProperty(propertyName, component->ComponentClass);
    if (!prop) {                
        UE_LOG(LogConfigLoader, Error, TEXT("Property %s from line %d was not found in Class %s"), *propertyName, node.GetMark().line +1, *component->ComponentClass->GetName());
        return;
    }

    void* defaultValue = nullptr;
    if (const auto defaultNode = node["Default"])
        defaultValue = LoadFPropertyValue(defaultNode, prop);

    if (component->Properties.Find(propertyName))
        UE_LOG(LogConfigLoader, Warning, TEXT("Property %s was already defined in component %s, replacing old definition"), *propertyName, *component->Name.ToString());
    component->AddProperty(
        propertyName,
        setter,
        prop,
        node["Required"].As<bool>(true),
        defaultValue
    );
}
void LoadComponent(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Component: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        const TSubclassOf<UActorComponent> componentClass = LoadClass(node["Class"]);
        if (!componentClass)
            return;

        const auto component = NewObject<UComponentInfo>(config.ObjOwner, FName(TEXT("Component_") + name.ToString()))->Init(
            name,
            componentClass
        );

        for (const auto& propertyNode : node["Properties"])
            LoadProperty(propertyNode, component);        
        for (const auto& need : node["Needs"])
            LoadNeed(need, component, config);

        if (config.Components.Contains(name.ToString()))
            UE_LOG(LogConfigLoader, Warning, TEXT("Component %s was already defined, replacing old definition"), *name.ToString());
        config.Components.Add(name.ToString(), component);
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Component: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadResource(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Resource: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        const auto resource = NewObject<UResource>(config.ObjOwner, FName(TEXT("Resource") + name.ToString()))->Init(
            name,
            *node["Image"].As<FString>(TEXT("/Game/Assets/Resources/Placeholder")),
            node["Description"].As<FText>(FText::FromString(TEXT("")))
        );

        if (resource) {
            if (config.Resources.Contains(name.ToString()))
                UE_LOG(LogConfigLoader, Warning, TEXT("Resource %s was already defined, replacing old definition"), *name.ToString());
            config.Resources.Add(name.ToString(), resource);
        }
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Resource: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadNaturalResource(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Natural Resource: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);
        
        UClass* uClass = LoadClass(node["Class"]);
        if (!uClass) {
            return;
        }

        const auto naturalResource = NewObject<UNaturalResource>(config.ObjOwner, FName(TEXT("NatResource_") + name.ToString()))->Init(
            uClass,
            name,
            *node["Image"].As<FString>(TEXT("/Game/Assets/Resources/Placeholder")),
            node["Description"].As<FText>(FText::FromString(TEXT("")))
        );

        if (naturalResource) {
            if (config.NaturalResources.Contains(name.ToString()))
                UE_LOG(LogConfigLoader, Warning, TEXT("Natural Resource %s was already defined, replacing old definition"), *name.ToString());
            config.NaturalResources.Add(name.ToString(), naturalResource);
        }
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Natural Resource: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}
void LoadBiome(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Biome: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        for (const auto naturalResource : node["NaturalResources"])
            LoadNaturalResource(naturalResource, config);
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Biome: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

UComponentLoader* LoadComponentRef(const FYamlNode& node, Config& config) {
    try {
        const auto component = LoadReference(node["Component"], config.Components);

        const auto componentLoader = NewObject<UComponentLoader>(config.ObjOwner)->Init(component);

        for (auto& propertyInfo : component->Properties) {
            if (auto propertyNode = node[propertyInfo.Key]) {
                auto& propInfo = propertyInfo.Value;
                componentLoader->SetProperty(&propInfo, LoadFPropertyValue(propertyNode, propInfo.Property));
            } else if (propertyInfo.Value.DefaultValue) {
                componentLoader->SetProperty(&propertyInfo.Value, propertyInfo.Value.DefaultValue);
            } else if (propertyInfo.Value.Required) {
                UE_LOG(LogConfigLoader, Error, TEXT("Error loading Component: Required Property %s not Found. In Block: %s, which starts on line %d"), *propertyInfo.Key, *node.GetContent(), node.GetMark().line +1);
                return nullptr;
            }
        }

        return componentLoader;
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Component: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
        return nullptr;
    }
}
TArray<UComponentLoader*> LoadComponentRefs(const FYamlNode& node, Config& config) {
    TArray<UComponentLoader*> components;
    for (const auto componentNode : node)
        if (const auto component = LoadComponentRef(componentNode, config))
            components.Add(component);
    return MoveTemp(components);
}
void LoadBuilding(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Building: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        UClass* uClass = ABuilding::StaticClass();
        if (const auto classNode = node["Class"])
            uClass = LoadClass(classNode);
        if (!uClass)
            return;

        const auto building = NewObject<UConstructionPlan>(config.ObjOwner,  FName(TEXT("Building_") + name.ToString()))->Init(
            uClass,
            name,
            *node["Image"].As<FString>(TEXT("/Game/Assets/Resources/Placeholder")),
            LoadComponentRefs(node["Components"], config),
            node["ConstructionTime"].As<int>(),
            LoadReferenceOpt(node["ConstructedOn"], config.NaturalResources),
            LoadMaterials<Material>(node["ConstructionMaterials"], config),
            node["Shape"].As<TArray<TPair<int, int>>>({}),
            node["Category"].As<FText>(FText::FromString(TEXT("MISSING"))),
            node["Description"].As<FText>(FText::FromString(TEXT("")))
        );

        if (building) {
            if (config.Buildings.Contains(name.ToString()))
                UE_LOG(LogConfigLoader, Warning, TEXT("Building %s was already defined, replacing old definition"), *name.ToString());
            config.Buildings.Add(name.ToString(), building);
        }
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Building: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadRecipe(const FYamlNode& node, Config& config) {
    try {
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Recipe (Line %d)"), node.GetMark().line +1);
        
        const auto recipe = NewObject<URecipe>(config.ObjOwner)->Init(
            LoadReferences(node["Buildings"], config.Buildings),
            LoadMaterials<Ingredient>(node["Ingredients"], config),
            LoadMaterials<Result>(node["Results"], config)
        );
        if (recipe)
            config.Recipes.Add(recipe);
        
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Recipe: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadNeed(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Need: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        const auto need = NewObject<UNeed>(config.ObjOwner,  FName(TEXT("Need_") + name.ToString()))->Init(
            name,
            *node["Image"].As<FString>(TEXT("/Game/Assets/Resources/Placeholder")),
            node["Description"].As<FText>(FText::FromString(TEXT(""))),
            node["Notification"].As<int>(),
            node["Start"].As<int>()
        );
        if (need)
            config.Needs.Add(name.ToString(), need);
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Need: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadNeedSatisfier(const FYamlNode& node, Config& config) {
    try {
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Need Satisfier (Line %d)"), node.GetMark().line +1);
        const auto satisfier = NewObject<UNeedSatisfier>(config.ObjOwner);
        satisfier->Need = LoadReference(node["Need"], config.Needs);

        for (const auto& good : node["Goods"])
            satisfier->Goods.Emplace(good["PeoplePerResource"].As<int>(), LoadReference(good["Resource"], config.Resources));
        for (const auto& service : node["Services"])
            satisfier->Services.Add(LoadReference(service["Building"], config.Buildings));

        config.NeedSatisfiers.Add(satisfier);
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading NeedSatisfier: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

template <class T>
UHotbarSlotAction* CreateBuilderModeSlot(UConstructionPlan* constructionPlan, Config& config) {
    if (constructionPlan->ConstructedOn) {
        UE_LOG(LogConfigLoader, Error, TEXT("Cannot add Building %s to the Hotbar because it has ConstructedOn set"), *constructionPlan->Name.ToString());
        return nullptr;
    }
    return CreateWidget<UHotbarSlotAction>(config.UIOwner, config.HotbarSlotActionUIClass)->Init(
        constructionPlan->Name,
        constructionPlan->Image,
        [constructionPlan, pawn = config.UIOwner->GetPawn<ACameraPawn>()] {
            if (pawn->PrepBuilderMode(constructionPlan->BuildingClass))
                pawn->BuilderMode = NewObject<T>(pawn)->Init(constructionPlan);
        }
    );
}
UHotbarSlot* LoadHotbarSlot(const FYamlNode& node, Config& config) {
    try {
        if (!node)
            return nullptr;

        if (const auto barNode = node["Bar"])
            if (const auto bar = LoadReference(barNode, config.Hotbars))
                if (const auto slot = CreateWidget<UHotbarSlotSubmenu>(config.UIOwner, config.HotbarSlotSubmenuUIClass)->Init(bar))
                    return slot;

        if (const auto builderMode = node["BuilderMode"])
            if (const auto building = LoadReference(builderMode, config.Buildings))
                if (const auto slot = CreateBuilderModeSlot<UBuildingBuilderMode>(building, config))
                    return slot;

        if (const auto builderMode = node["IndoorBuilderMode"])
            if (const auto building = LoadReference(builderMode, config.Buildings))
                if (const auto slot = CreateBuilderModeSlot<UIndoorBuilderMode>(building, config))
                    return slot;

        if (const auto builderMode = node["ConveyorBuilderMode"]) {
            if (const auto building = config.Buildings.Find(TEXT("Conveyor"))) {
                if (const auto slot = CreateBuilderModeSlot<UConveyorBuilderMode>(*building, config))
                    return slot;
            } else
                UE_LOG(LogConfigLoader, Error, TEXT("Unable to find Building \"Conveyor\", which is required for ConveyorBuilderMode as referenced on line %d"), builderMode.GetMark().line +1);
        }

        if (const auto builderMode = node["DismantleMode"]) {            
            return CreateWidget<UHotbarSlotAction>(config.UIOwner, config.HotbarSlotActionUIClass)->Init(
                FText::FromString(TEXT("Dismantle")),
                LoadObject<UTexture2D>(nullptr, TEXT("/Game/Assets/Images/Dismantle")),
                [pawn = config.UIOwner->GetPawn<ACameraPawn>()] {
                    if (pawn->PrepBuilderMode(UDismantleMode::StaticClass()))
                        pawn->BuilderMode = NewObject<UDismantleMode>(pawn)->Init();
                }
            );
        }

        UE_LOG(LogConfigLoader, Warning, TEXT("Can't create a Hotbar Slot from Block %s on line %d"), *node.GetContent(), node.GetMark().line +1);
        return nullptr;
     
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Hotbar Slot: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
        return nullptr;
    }
}
void LoadHotbar(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Hotbar: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        const auto hotbar = CreateWidget<UHotbar>(config.UIOwner, config.HotbarUIClass, FName(*name.ToString()))->Init(
            LoadHotbarSlot(node["Slot1"], config),
            LoadHotbarSlot(node["Slot2"], config),
            LoadHotbarSlot(node["Slot3"], config),
            LoadHotbarSlot(node["Slot4"], config),
            LoadHotbarSlot(node["Slot5"], config),
            LoadHotbarSlot(node["Slot6"], config),
            LoadHotbarSlot(node["Slot7"], config),
            LoadHotbarSlot(node["Slot8"], config),
            LoadHotbarSlot(node["Slot9"], config),
            LoadHotbarSlot(node["Slot0"], config)
        );

        if (config.Hotbars.Contains(name.ToString()))
            UE_LOG(LogConfigLoader, Warning, TEXT("Hotbar %s was already defined, replacing old definition"), *name.ToString());
        config.Hotbars.Add(name.ToString(), hotbar);
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Hotbar: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}
void LoadHotbars(const FYamlNode& node, Config& config) {
    for (const auto& bar : node["Bars"])
        LoadHotbar(bar, config);

    if (const auto main = node["Main"]) {
        try {
            if (const auto bar = LoadReference(main, config.Hotbars)) {
                if (config.MainHotbar)
                    UE_LOG(LogConfigLoader, Warning, TEXT("Main Hotbar was already defined, replacing old definition"));
                config.MainHotbar = bar;
            }
        } catch (YAML::Exception e) {
            UE_LOG(LogConfigLoader, Error, TEXT("Error loading Main Hotbar: %hs in Block: %s, which starts on line %d"), e.what(), *main.GetContent(), main.GetMark().line +1);
        }
    }
}

void LoadEncyclopediaPage(const FYamlNode& node, Config& config) {    
    try {
        const auto name = node["Title"].As<FText>();
        UE_LOG(LogConfigLoader, Display, TEXT("Loading Encyclopedia Page %s (Line %d)"), *name.ToString(), node.GetMark().line +1);
        config.EncyclopediaPages.Emplace(name, node["Text"].As<FText>());
    } catch (YAML::Exception e) {
        UE_LOG(LogConfigLoader, Error, TEXT("Error loading Encyclopedia Page: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadFile(FString& path, Config& config) {
    FPaths::NormalizeFilename(path);
    FPaths::CollapseRelativeDirectories(path);

    FYamlNode root;
    if (!UYamlParsing::LoadYamlFromFile(path, root)) {
        UE_LOG(LogConfigLoader, Error, TEXT("Failed to load Config File: %s"), *path);
        return;
    }

    UE_LOG(LogConfigLoader, Display, TEXT("Loading Config File: %s"), *path);

    for (const auto& resource : root["Resources"]) 
        LoadResource(resource, config);

    for (const auto& component : root["Components"]) 
        LoadComponent(component, config);

    for (const auto& biome : root["Biomes"]) 
        LoadBiome(biome, config);

    for (const auto& building : root["Buildings"]) 
        LoadBuilding(building, config);

    for (const auto& recipe : root["Recipes"]) 
        LoadRecipe(recipe, config);

    for (const auto& need : root["Needs"]) 
        LoadNeed(need, config);

    for (const auto& needSatisfier : root["NeedSatisfiers"]) 
        LoadNeedSatisfier(needSatisfier, config);

    if (const auto hotbars = root["Hotbars"])
        LoadHotbars(hotbars, config);

    for (const auto& encyclopediaPage : root["EncyclopediaPages"])
        LoadEncyclopediaPage(encyclopediaPage, config);

    const auto folder = FPaths::GetPath(path);
    for (const auto& file : root["Files"]) {
        try {
            auto filePath = FPaths::Combine(folder, file.As<FString>());
            LoadFile(filePath, config);
        } catch (YAML::Exception e) {
            UE_LOG(LogConfigLoader, Error, TEXT("Error loading File Name: %hs in Block: %s, which starts on line %d"), e.what(), *file.GetContent(), file.GetMark().line +1);
        }
    }
}

TPair<UEncyclopedia*, TArray<TPair<FText, FText>>> ConfigLoader::Load(const UHotbarDock* dock) {
    const auto encyclopedia = NewObject<UEncyclopedia>();
    Config config(encyclopedia, dock->GetOwningPlayer<APlayerControllerX>(), dock->HotbarUIClass, dock->HotbarSlotSubmenuUIClass, dock->HotbarSlotActionUIClass);
    auto path = FPaths::ProjectDir() + "Content/Config/root.yml"; // TODO there are some other interesting options in FPaths, test how this behaves on a release build
    LoadFile(path, config);

    if (config.MainHotbar)
        dock->SetMainHotbar(config.MainHotbar);

    return MakeTuple(
        encyclopedia->Init(config.Resources, config.NaturalResources, config.Buildings, config.Recipes, config.Needs, config.NeedSatisfiers),
        MoveTemp(config.EncyclopediaPages)
    );
}