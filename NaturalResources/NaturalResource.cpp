// Fill out your copyright notice in the Description page of Project Settings.

#include "NaturalResource.h"

#include "The.h"
#include "XD/Construction/BuilderModeExtension.h"
#include "XD/Construction/ConstructionSite.h"

UConstructionSite* ANaturalResourceActor::Construct() {   
    const auto encyclopedia = The::Encyclopedia(this);
    const auto constructionPlan = encyclopedia->GetBuildings(encyclopedia->GetNaturalResource(GetClass()))[0];

    const auto building = GetWorld()->SpawnActor<ABuilding>(
        constructionPlan->BuildingClass,
        // TODO figure out why that -50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        GetActorLocation() - FVector(.0f, .0f, 50.f),
        GetActorRotation()
    )->Init(constructionPlan);

    const auto constructionSite = NewObject<UConstructionSite>(building)->Init(building, constructionPlan, NewObject<UBuilderModeExtensions>());
    this->Destroy();
    constructionSite->QueueTasks();
    return constructionSite;
}

UNaturalResource* UNaturalResource::Init(UClass* buildingClass, const FText& name, const TCHAR* image, const FText& description) {
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Natural Resource, Name is blank: %s"), *name.ToString());
        return nullptr;
    }
    BuildingClass = buildingClass;
    Name = name;
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load Texture %s"), image);
    Description = description;
    return this;
}

bool UNaturalResource::operator<(const UNaturalResource& other) const {
    return Name.CompareTo(other.Name) < 0;
}

void UNaturalResource::Respawn(const ABuilding* building) const {
    building->GetWorld()->SpawnActor<ANaturalResourceActor>(
        BuildingClass,
        // TODO figure out why that 50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        building->GetActorLocation() - FVector(.0f, .0f, 50.f),
        building->GetActorRotation()
    );
}
