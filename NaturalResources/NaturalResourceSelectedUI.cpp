// Fill out your copyright notice in the Description page of Project Settings.

#include "NaturalResourceSelectedUI.h"

#include "The.h"
#include "XD/PlayerControllerX.h"
#include "XD/Encyclopedia/Encyclopedia.h"

UNaturalResourceSelectedUI* UNaturalResourceSelectedUI::Init(ANaturalResourceActor* actor) {
    const auto encyclopedia = The::Encyclopedia(actor);

    const auto naturalResource = encyclopedia->GetNaturalResource(actor->GetClass());
    check(naturalResource);
    if (!naturalResource)
        return nullptr;

    Selected = actor;
    NaturalResource->Init(naturalResource);
    Name->SetText(naturalResource->Name);

    const auto& buildings = encyclopedia->GetBuildings(naturalResource);
    check(buildings.Num() == 1); // TODO create a pop up dialog if there is more then 1 building
    if (buildings.IsEmpty()) {
        // TODO hide Building
        // TODO disable button
        ConstructionMaterials->Clear();
        return this;
    }
    // TODO unhide building
    // TODO enable button

    const auto building = buildings[0];
    Building->Init(building);
    ConstructionMaterials->Init(building->Materials, The::ConstructionManager(actor));

    for (const auto need : building->GetNeeds())
        if (need.amount > 0)
            ConstructionMaterials->AddExternalResource(CreateWidget<UResourceAmountUI>(this, ResourceAmountUIClass)->Init(need.amount, need.resource));
    
    return this;
}

void UNaturalResourceSelectedUI::Tick() {
    ConstructionMaterials->UpdateHave(The::ConstructionManager(Selected));
}

void UNaturalResourceSelectedUI::OnClickConstruct() {
    if (!Selected)
        return;

    const auto encyclopedia = The::Encyclopedia(Selected);
    const auto constructionPlan = encyclopedia->GetBuildings(encyclopedia->GetNaturalResource(Selected->GetClass()))[0];

    const auto building = GetWorld()->SpawnActor<ABuilding>(
        constructionPlan->BuildingClass,
        // TODO figure out why that -50 is needed, could be because of the cylinder placeholder model or maybe collision checking?
        Selected->GetActorLocation() - FVector(.0f, .0f, 50.f),
        Selected->GetActorRotation()
    )->Init(constructionPlan);

    The::ConstructionManager(Selected)->AddConstruction(
        NewObject<UConstructionSite>()->Init(building, constructionPlan, NewObject<UBuilderModeExtensions>())
    );
    The::PlayerController(Selected)->Deselect(); // TODO once we have an UI for construction sites, we can select that instead
    Selected->Destroy();
    Selected = nullptr;
}
