// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaUI.h"

#include "Collections.h"
#include "UI.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Resources/Resource.h"

UEncyclopediaCategory* UEncyclopediaUI::AddCategory(const FString& name) const {
    const auto category = CreateWidget<UEncyclopediaCategory>(
        GetOwningPlayer(),
        EncyclopediaCategoryClass)->Init(FText::FromString(name));
    Categories->AddChild(category);
    return category;
}

UEncyclopediaPageResource* UEncyclopediaUI::CreateResourcePage(UResource* resource) const {
    return CreateWidget<UEncyclopediaPageResource>(
        GetOwningPlayer(),
        EncyclopediaPageResourceClass)->Init(resource, Encyclopedia);
}
UEncyclopediaPageNaturalResource* UEncyclopediaUI::CreateNaturalResourcePage(UNaturalResource* naturalResource) const {
    return CreateWidget<UEncyclopediaPageNaturalResource>(
        GetOwningPlayer(),
        EncyclopediaPageNaturalResourceClass)->Init(naturalResource, Encyclopedia);
}
UEncyclopediaPageBuilding* UEncyclopediaUI::CreateBuildingPage(UConstructionPlan* building) const {
    return CreateWidget<UEncyclopediaPageBuilding>(
        GetOwningPlayer(),
        EncyclopediaPageBuildingClass)->Init(building, Encyclopedia);
}

void UEncyclopediaUI::Fill(UEncyclopedia* encyclopedia) {
    check(encyclopedia);
    Encyclopedia = encyclopedia;
    Categories->ClearChildren();

    {
        const auto settings = AddCategory(TEXT("Settings"));
        settings->AddEntry(TEXT("Settings"), nullptr); // TODO add Page
        // TODO wenn das wirklich nur 1 item ist dann könnte ich das auch top level ohne category machen
    }

    {
        const auto concepts = AddCategory(TEXT("Concepts"));
        concepts->AddEntry(TEXT("The Encyclopedia"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Natural Resources"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Construction"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Conveyors"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Splitters & Mergers"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Production & Recipes"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Electricity"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Habitats"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("People & Workforce"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Exploration"), nullptr); // TODO add Page
        concepts->AddEntry(TEXT("Research"), nullptr); // TODO add Page
    }

    {
        const auto resources = AddCategory(TEXT("Resources"));

        const auto rawMaterialsSet = encyclopedia->FindRawMaterials();
        const auto constructionResourcesSet = encyclopedia->FindConstructionResources();
        const auto needsSet = encyclopedia->FindNeeds();

        TArray<UResource*> intermediateProducts;
        intermediateProducts.Append(encyclopedia->GetAllResources());
        intermediateProducts.RemoveAllSwap([&rawMaterialsSet](const UResource* it){return rawMaterialsSet.Contains(it);});
        intermediateProducts.RemoveAllSwap([&constructionResourcesSet](const UResource* it){return constructionResourcesSet.Contains(it);});
        intermediateProducts.RemoveAllSwap([&needsSet](const UResource* it){return needsSet.Contains(it);});
        intermediateProducts.RemoveSwap(encyclopedia->People);
        intermediateProducts.RemoveSwap(encyclopedia->Workforce);
        intermediateProducts.RemoveSwap(encyclopedia->Food);
        
        TArray<UResource*> rawMaterials;    
        for (const auto resource : rawMaterialsSet)
            rawMaterials.Add(resource);
        TArray<UResource*> constructionResources;
        for (const auto resource : constructionResourcesSet)
            constructionResources.Add(resource);
        TArray<UResource*> needs;
        for (const auto resource : needsSet)
            needs.Add(resource);

        rawMaterials.Sort();
        constructionResources.Sort();
        needs.Sort();
        intermediateProducts.Sort();

        const auto rawCategory = resources->AddSubCategory(TEXT("Raw Materials"));
        for (const auto resource : rawMaterials)
            resource->EncyclopediaEntry = rawCategory->AddEntry(resource->Name.ToString(), CreateResourcePage(resource));
        
        // TODO should somehow redirect to people & workforce
        encyclopedia->Food->EncyclopediaEntry = rawCategory->AddEntry(encyclopedia->Food->Name.ToString(), nullptr); // TODO add Page

        const auto intermediateCategory = resources->AddSubCategory(TEXT("Intermediate Products"));
        for (const auto resource : intermediateProducts)
            resource->EncyclopediaEntry = intermediateCategory->AddEntry(resource->Name.ToString(), CreateResourcePage(resource));
        
        const auto constructionCategory = resources->AddSubCategory(TEXT("Construction Materials"));
        for (const auto resource : constructionResources)
            resource->EncyclopediaEntry = constructionCategory->AddEntry(resource->Name.ToString(), CreateResourcePage(resource));

        const auto needsCategory = resources->AddSubCategory(TEXT("Needs"));
        for (const auto resource : needs)
            resource->EncyclopediaEntry = needsCategory->AddEntry(resource->Name.ToString(), CreateResourcePage(resource));

        const auto peoplePage = needsCategory->AddEntry(TEXT("People & Workforce"), nullptr); // TODO add special page
        encyclopedia->People->EncyclopediaEntry = peoplePage;
        encyclopedia->Workforce->EncyclopediaEntry = peoplePage;
    }

    {
        const auto naturalResources = AddCategory(TEXT("Natural Resources"));
        for (const auto naturalResource : encyclopedia->GetAllNaturalResources())
            naturalResource->EncyclopediaEntry = naturalResources->AddEntry(naturalResource->Name.ToString(), CreateNaturalResourcePage(naturalResource));
    }

    {
        const auto buildings = AddCategory(TEXT("Buildings"));

        TMap<FString, TArray<UConstructionPlan*>> categories;
        for (const auto building : encyclopedia->GetAllBuildings())
            MultiMaps::AddTo(categories, building->Category.ToString(), building);

        TArray<FString> categoryNames;
        categories.GenerateKeyArray(categoryNames);
        categoryNames.Sort();

        for (const auto& categoryName : categoryNames) {
            if (categoryName.Equals(TEXT("hidden")))
                continue;            
            auto category = buildings->AddSubCategory(categoryName);

            for (const auto building : MultiMaps::Find(categories, categoryName)) {
                if (building == encyclopedia->Conveyor) {
                    building->EncyclopediaEntry = category->AddEntry(building->Name.ToString(), nullptr); // TODO add special conveyor Page
                    encyclopedia->ConveyorNode->EncyclopediaEntry = building->EncyclopediaEntry;
                    encyclopedia->ConveyorLink->EncyclopediaEntry = building->EncyclopediaEntry;
                } else
                    building->EncyclopediaEntry = category->AddEntry(building->Name.ToString(), CreateBuildingPage(building));
            }
        }
    }

    CollapseAll();
}

void UEncyclopediaUI::Open() {
    AddToViewport();
    GetOwningPlayer()->InputComponent->BindAction("Deselect", IE_Pressed, this, &UEncyclopediaUI::Close);
}
void UEncyclopediaUI::Close() {
    RemoveFromParent();
    GetOwningPlayer()->InputComponent->RemoveActionBinding("Deselect", IE_Pressed);
}

void UEncyclopediaUI::OpenPage(UEncyclopediaEntry* entry) {
    if (!IsVisible())
        Open();

    if (entry == OpenedEntry)
        return;

    ClosePage();
    if (!entry)
        return;

    entry->SetOpen(true);
    Title->SetText(entry->GetTitle());
    SetContentForSlot(TEXT("Page"), entry->Page);

    UEncyclopediaCategory* oldCat = nullptr;
    UEncyclopediaCategory* currentCat = UI::GetParentWidget<UEncyclopediaCategory>(entry);
    while(currentCat) {
        currentCat->CollapseAll(oldCat, false);
        currentCat->ExpandableArea->SetIsExpanded_Animated(true);
        oldCat = currentCat;
        currentCat = UI::GetParentWidget<UEncyclopediaCategory>(currentCat);
    }
    CollapseAll(oldCat);

    OpenedEntry = entry;
}

void UEncyclopediaUI::ClosePage() {
    if (!OpenedEntry)
        return;

    OpenedEntry->SetOpen(false);
    OpenedEntry = nullptr;
    Title->SetText(FText::FromString(TEXT("Encyclopedia")));
    SetContentForSlot(TEXT("Page"), nullptr);

    OpenedEntry = nullptr;
}

void UEncyclopediaUI::CollapseAll(UEncyclopediaCategory* ignore) const {    
    for (const auto child : Categories->GetAllChildren())
        if (const auto subCategory = Cast<UEncyclopediaCategory>(child))
            if (subCategory != ignore)
                subCategory->CollapseAll(ignore);
}

UEncyclopediaCategory* UEncyclopediaCategory::Init(const FText& label) {
    Label->SetText(label);
    Entries->ClearChildren();
    return this;
}

UEncyclopediaEntry* UEncyclopediaCategory::AddEntry(const FString& title, UEncyclopediaPage* page) const {
    const auto encyclopedia = UI::GetParentWidget<UEncyclopediaUI>(this);
    const auto entry = CreateWidget<UEncyclopediaEntry>(
        GetOwningPlayer(),
        encyclopedia->EncyclopediaEntryClass)->Init(FText::FromString(title), page);
    Entries->AddChild(entry);
    return entry;
}

UEncyclopediaCategory* UEncyclopediaCategory::AddSubCategory(const FString& name) const {
    const auto encyclopedia = UI::GetParentWidget<UEncyclopediaUI>(this);
    const auto category = CreateWidget<UEncyclopediaCategory>(
        GetOwningPlayer(),
        encyclopedia->EncyclopediaCategoryClass)->Init(FText::FromString(name));
    Entries->AddChild(category);
    return category;
}

void UEncyclopediaCategory::CollapseAll(UEncyclopediaCategory* ignore, bool includeSelf) const {
    for (const auto child : Entries->GetAllChildren())
        if (const auto subCategory = Cast<UEncyclopediaCategory>(child))
            if (subCategory != ignore)
                subCategory->CollapseAll(ignore);
    if (includeSelf)
        ExpandableArea->SetIsExpanded(false);
}

UEncyclopediaEntry* UEncyclopediaEntry::Init(const FText& label, UEncyclopediaPage* page) {
    Title->SetText(label);
    Page = page;
    return this;
}

void UEncyclopediaEntry::SetHighlight(bool highlight) const {
    if (Opened)
        return;

    if (highlight)
        Title->SetColorAndOpacity(HighlightColor);
    else
        Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
}

void UEncyclopediaEntry::OnClicked() {
    UI::GetParentWidget<UEncyclopediaUI>(this)->OpenPage(this);
}

void UEncyclopediaEntry::SetOpen(bool open) {
    if (open == Opened)
        return;

    Opened = false;
    SetHighlight(open);
    Opened = open;
}

FText UEncyclopediaEntry::GetTitle() const {
    return Title->GetText();
}
