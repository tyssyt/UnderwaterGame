// Fill out your copyright notice in the Description page of Project Settings.

#include "WorkforceComponent.h"

#include "The.h"
#include "UI.h"
#include "XD/BlueprintHolder.h"

UNoWorkers::UNoWorkers() {
    const static ConstructorHelpers::FObjectFinder<UTexture2D> SymbolFinder(TEXT("/Game/Assets/Images/NoWorkers"));
    Symbol = SymbolFinder.Object;
    Type = !UObject::GetWorld() || The::Cheats(this)->AlwaysPowered ? EType::Visual : EType::TickDisabled;
}

UWorkforceComponent::UWorkforceComponent() {
    PrimaryComponentTick.bCanEverTick = false;
}

UResource* UWorkforceComponent::GetWorkforce() const {
    return ComponentInfo->Needs[0].Resource;
}

void UWorkforceComponent::SetStaffed() {
    check(State != EWorkforceState::Staffed);
    State = EWorkforceState::Staffed;
    if (Condition) {
        GetOwner<ABuilding>()->RemoveCondition(Condition);
        Condition = nullptr;
    }
}
void UWorkforceComponent::SetUnstaffed() {
    check(State != EWorkforceState::Unstaffed);
    State = EWorkforceState::Unstaffed;
    Condition = NewObject<UNoWorkers>(GetOwner());
    GetOwner<ABuilding>()->AddCondition(Condition);
}

UBuilderModeExtension* UWorkforceComponent::CreateBuilderModeExtension() {
    return NewObject<UWorkforceBuilderModeExtension>(GetOwner());
}

void UWorkforceComponent::OnConstructionComplete(UBuilderModeExtension* extension) {
    The::PopulationManager(this)->AddBuilding(this);
}

void UWorkforceComponent::OnDismantle() {
    The::PopulationManager(this)->RemoveBuilding(this);
}

void UWorkforceComponent::AddToSelectedUI(TArray<UBuildingSelectedUIComponent*>& components) {
    components.Add(NewObject<UWorkforceComponentUI>(GetOwner())->Init(this));
}

UWorkforceComponentUI* UWorkforceComponentUI::Init(UWorkforceComponent* workforceComponent) {
    WorkforceComponent = workforceComponent;
    return this;
}

void UWorkforceComponentUI::CreateUI(UBuildingSelectedUI* selectedUI) {
    UI = CreateWidget<UResourceBalanceUI>(
        The::PlayerController(this),
        The::BPHolder(this)->ResourceBalanceUIClass);
    UI->SetNeed(WorkforceComponent->Workers, WorkforceComponent->GetWorkforce());
    UI->SetHave(The::PopulationManager(this)->Workforce);
    selectedUI->Top->AddChildToWrapBox(UX::Sized(selectedUI->WidgetTree, UI, 60.f, 60.f));
}

void UWorkforceComponentUI::Tick(UBuildingSelectedUI* selectedUI) {
    UI->SetHave(The::PopulationManager(this)->Workforce);
}

void UWorkforceBuilderModeExtension::Init(ABuilding* preview, UConstructionUI* constructionUI) {        
    const auto workforce = preview->GetComponentByClass<UWorkforceComponent>();
    check(workforce);

    Preview = preview;

    const auto constructionMaterials = constructionUI->ConstructionMaterials;
    ResourceUI = CreateWidget<UResourceBalanceUI>(constructionMaterials, constructionMaterials->ResourceBalanceUIClass);
    ResourceUI->SetNeed(workforce->Workers, workforce->GetWorkforce());
    ResourceUI->SetHave(The::PopulationManager(this)->GetSurplusWorkforce());
    constructionMaterials->AddExternalResource(ResourceUI);
}

void UWorkforceBuilderModeExtension::Update() {
    ResourceUI->SetHave(The::PopulationManager(this)->GetSurplusWorkforce());
}
