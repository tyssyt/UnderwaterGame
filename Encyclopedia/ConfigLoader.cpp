// Fill out your copyright notice in the Description page of Project Settings.

#include "ConfigLoader.h"

#include "Encyclopedia.h"
#include "Node.h"
#include "Parsing.h"
#include "XD/CameraPawn.h"
#include "XD/PlayerControllerX.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Construction/BuilderModeExtension.h"
#include "XD/Construction/BuildingBuilderMode.h"
#include "XD/Construction/ConveyorBuilderMode.h"
#include "XD/Construction/IndoorBuilderMode.h"
#include "XD/Hotbar/Hotbar.h"
#include "XD/Hotbar/HotbarDock.h"
#include "XD/Hotbar/HotbarSlotAction.h"
#include "XD/Hotbar/HotbarSlotSubmenu.h"
#include "XD/Recipes/Recipe.h"
#include "XD/Resources/Resource.h"

struct Config {
    UObject* ObjOwner;
    APlayerControllerX* UIOwner;
    const TSubclassOf<UHotbar> HotbarUIClass;
    const TSubclassOf<UHotbarSlotSubmenu> HotbarSlotSubmenuUIClass;
    const TSubclassOf<UHotbarSlotAction> HotbarSlotActionUIClass;

    // TODO maybe I want FName instead of FString here
    TMap<FString, UResource*> Resources;
    TMap<FString, UConstructionPlan*> Buildings;
    TArray<URecipe*> Recipes;
    TMap<FString, UHotbar*> Hotbars;
    UHotbar* MainHotbar;

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

template<class T>
T* LoadReference(const FYamlNode& node, TMap<FString, T*>& map) {
    const FString name = node.As<FString>();
    if (const auto reference = map.Find(name))
        return *reference;
    UE_LOG(LogTemp, Error, TEXT("Unable to find %s %s referenced on line %d"), *T::StaticClass()->GetName(), *name, node.GetMark().line +1);
    return nullptr;
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
        UE_LOG(LogTemp, Error, TEXT("Failed to load UClass: %s in Block: %s, which starts on line %d"), *node["Class"].As<FString>(), *node.GetContent(), node.GetMark().line +1);  
    return uClass;
}

template<class T>
TOptional<T> LoadMaterial(const FYamlNode& node, Config& config) {
    try {
        if (const auto resource = LoadReference(node["Resource"], config.Resources))
            return TOptional<T>(InPlace, node["Amount"].As<int>(), resource);
        return {};
    } catch (YAML::Exception e) {
        UE_LOG(LogTemp, Error, TEXT("Error loading Material: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
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

void LoadResource(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogTemp, Display, TEXT("Loading Resource: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        const auto resource = NewObject<UResource>(config.ObjOwner, FName(*name.ToString()))->Init(
            name,
            *node["Image"].As<FString>(TEXT("/Game/Assets/Resources/Placeholder")),
            node["Description"].As<FText>(FText::FromString(TEXT("")))
        );

        if (resource) {
            if (config.Resources.Contains(name.ToString()))
                UE_LOG(LogTemp, Warning, TEXT("Resource %s was already defined, replacing old definition"), *name.ToString());
            config.Resources.Add(name.ToString(), resource);
        }
    } catch (YAML::Exception e) {
        UE_LOG(LogTemp, Error, TEXT("Error loading Resource: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadBuilding(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogTemp, Display, TEXT("Loading Building: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

        UClass* uClass = LoadClass(node["Class"]);
        if (!uClass) {
            return;
        }

        TSubclassOf<UBuilderModeExtension> builderModeExtension = nullptr;
        if (const auto builderModeExtensionNode = node["BuilderModeExtension"])
            builderModeExtension = LoadClass(node["BuilderModeExtension"]);

        const auto building = NewObject<UConstructionPlan>(config.ObjOwner, FName(*name.ToString()))->Init(
            uClass,
            builderModeExtension,
            name,
            *node["Image"].As<FString>(TEXT("/Game/Assets/Resources/Placeholder")),
            node["ConstructionTime"].As<int>(),
            LoadMaterials<Material>(node["ConstructionMaterials"], config),
            node["Description"].As<FText>(FText::FromString(TEXT("")))
        );

        if (building) {
            if (config.Buildings.Contains(name.ToString()))
                UE_LOG(LogTemp, Warning, TEXT("Building %s was already defined, replacing old definition"), *name.ToString());
            config.Buildings.Add(name.ToString(), building);
        }
    } catch (YAML::Exception e) {
        UE_LOG(LogTemp, Error, TEXT("Error loading Building: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}

void LoadRecipe(const FYamlNode& node, Config& config) {
    try {
        UE_LOG(LogTemp, Display, TEXT("Loading Recipe (Line %d)"), node.GetMark().line +1);
        
        const auto recipe = NewObject<URecipe>(config.ObjOwner)->Init(
            LoadReferences( node["Buildings"], config.Buildings),
            LoadMaterials<Ingredient>(node["Ingredients"], config),
            LoadMaterials<Result>(node["Results"], config)
        );
        if (recipe)
            config.Recipes.Add(recipe);
        
    } catch (YAML::Exception e) {
        UE_LOG(LogTemp, Error, TEXT("Error loading Recipe: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}
        
template <class T>
UHotbarSlotAction* CreateBuilderModeSlot(UConstructionPlan* constructionPlan, Config& config) {
    return CreateWidget<UHotbarSlotAction>(config.UIOwner, config.HotbarSlotActionUIClass)->Init(constructionPlan->Image, [constructionPlan, pawn = config.UIOwner->GetPawn<ACameraPawn>()]{
        if (pawn->PrepBuilderMode(constructionPlan->BuildingClass))
            pawn->BuilderMode = NewObject<T>(pawn)->Init(constructionPlan);
    });
}
UHotbarSlot* LoadHotbarSlot(const FYamlNode& node, Config& config) {
    try {
        if (!node)
            return nullptr;

        if (const auto barNode = node["Bar"])
            if (const auto bar = LoadReference(barNode, config.Hotbars))
                return CreateWidget<UHotbarSlotSubmenu>(config.UIOwner, config.HotbarSlotSubmenuUIClass)->Init(bar);

        if (const auto builderMode = node["BuilderMode"])
            if (const auto building = LoadReference(builderMode, config.Buildings))
                return CreateBuilderModeSlot<UBuildingBuilderMode>(building, config);

        if (const auto builderMode = node["IndoorBuilderMode"])
            if (const auto building = LoadReference(builderMode, config.Buildings))
                return CreateBuilderModeSlot<UIndoorBuilderMode>(building, config);

        if (const auto builderMode = node["ConveyorBuilderMode"]) {
            if (const auto building = config.Buildings.Find(TEXT("Conveyor Link")))
                return CreateBuilderModeSlot<UConveyorBuilderMode>(*building, config);
            else
                UE_LOG(LogTemp, Error, TEXT("Unable to find Building \"Conveyor Link\", which is required for ConveyorBuilderMode as referenced on line %d"), builderMode.GetMark().line +1);
        }

        UE_LOG(LogTemp, Warning, TEXT("Can't create a Hotbar Slot from Block %s on line %d"), *node.GetContent(), node.GetMark().line +1);
        return nullptr;
     
    } catch (YAML::Exception e) {
        UE_LOG(LogTemp, Error, TEXT("Error loading Hotbar Slot: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
        return nullptr;
    }
}
void LoadHotbar(const FYamlNode& node, Config& config) {
    try {
        const auto name = node["Name"].As<FText>();
        UE_LOG(LogTemp, Display, TEXT("Loading Hotbar: %s (Line %d)"), *name.ToString(), node.GetMark().line +1);

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
            UE_LOG(LogTemp, Warning, TEXT("Hotbar %s was already defined, replacing old definition"), *name.ToString());
        config.Hotbars.Add(name.ToString(), hotbar);
    } catch (YAML::Exception e) {
        UE_LOG(LogTemp, Error, TEXT("Error loading Hotbar: %hs in Block: %s, which starts on line %d"), e.what(), *node.GetContent(), node.GetMark().line +1);
    }
}
void LoadHotbars(const FYamlNode& node, Config& config) {
    for (const auto& bar : node["Bars"])
        LoadHotbar(bar, config);

    if (const auto main = node["Main"]) {
        try {
            if (const auto bar = LoadReference(main, config.Hotbars)) {
                if (config.MainHotbar)
                    UE_LOG(LogTemp, Warning, TEXT("Main Hotbar was already defined, replacing old definition"));
                config.MainHotbar = bar;
            }
        } catch (YAML::Exception e) {
            UE_LOG(LogTemp, Error, TEXT("Error loading Main Hotbar: %hs in Block: %s, which starts on line %d"), e.what(), *main.GetContent(), main.GetMark().line +1);
        }
    }
}


void LoadFile(FString& path, Config& config) {
    FPaths::NormalizeFilename(path);
    FPaths::CollapseRelativeDirectories(path);

    FYamlNode root;
    if (!UYamlParsing::LoadYamlFromFile(path, root)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Config File: %s"), *path);
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Loading Config File: %s"), *path);
    
    for (const auto& resource : root["Resources"]) 
        LoadResource(resource, config);
    
    for (const auto& building : root["Buildings"]) 
        LoadBuilding(building, config);
    
    for (const auto& recipe : root["Recipes"]) 
        LoadRecipe(recipe, config);

    if (const auto hotbars = root["Hotbars"])
        LoadHotbars(hotbars, config);

    const auto folder = FPaths::GetPath(path);
    for (const auto& file : root["Files"]) {
        try {
            auto filePath = FPaths::Combine(folder, file.As<FString>());
            LoadFile(filePath, config);
        } catch (YAML::Exception e) {
            UE_LOG(LogTemp, Error, TEXT("Error loading File Name: %hs in Block: %s, which starts on line %d"), e.what(), *file.GetContent(), file.GetMark().line +1);
        }
    }
}

UEncyclopedia* ConfigLoader::Load(const UHotbarDock* dock) {
    const auto encyclopedia = NewObject<UEncyclopedia>();
    Config config(encyclopedia, dock->GetOwningPlayer<APlayerControllerX>(), dock->HotbarUIClass, dock->HotbarSlotSubmenuUIClass, dock->HotbarSlotActionUIClass);
    auto path = FPaths::ProjectDir() + "Content/Config/root.yml"; // TODO there are some other interesting options in FPaths, test how this behaves on a release build
    LoadFile(path, config);

    if (config.MainHotbar)
        dock->SetMainHotbar(config.MainHotbar);

    return encyclopedia->Init(config.Resources, config.Buildings, config.Recipes);
}