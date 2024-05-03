// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaUI.h"

#include "Collections.h"
#include "EncyclopediaPageNeed.h"
#include "EncyclopediaPageText.h"
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

UEncyclopediaEntry* UEncyclopediaUI::CreateAndAddResourcePage(const UEncyclopediaCategory* category, UResource* resource) {
    const auto page = CreateWidget<UEncyclopediaPageResource>(
        GetOwningPlayer(),
        EncyclopediaPageResourceClass)->Init(resource, Encyclopedia);
    return AddPage(category, resource->Name, page);
}
UEncyclopediaEntry* UEncyclopediaUI::CreateAndAddNaturalResourcePage(const UEncyclopediaCategory* category,UNaturalResource* naturalResource) {
    const auto page = CreateWidget<UEncyclopediaPageNaturalResource>(
        GetOwningPlayer(),
        EncyclopediaPageNaturalResourceClass)->Init(naturalResource, Encyclopedia);
    return AddPage(category, naturalResource->Name, page);
}
UEncyclopediaEntry* UEncyclopediaUI::CreateAndAddBuildingPage(const UEncyclopediaCategory* category, UConstructionPlan* building) {
    const auto page = CreateWidget<UEncyclopediaPageBuilding>(
        GetOwningPlayer(),
        EncyclopediaPageBuildingClass)->Init(building, Encyclopedia);
    return AddPage(category, building->Name, page);
}
UEncyclopediaEntry* UEncyclopediaUI::CreateAndAddNeedPage(const UEncyclopediaCategory* category, UNeed* need) {
    const auto page = CreateWidget<UEncyclopediaPageNeed>(
        GetOwningPlayer(),
        EncyclopediaPageNeedClass)->Init(need, Encyclopedia);
    return AddPage(category, need->Name, page);
}
UEncyclopediaEntry* UEncyclopediaUI::CreateAndAddTextPage(const UEncyclopediaCategory* category, const FText& title, const FText& text) {
    const auto page = CreateWidget<UEncyclopediaPageText>(
    GetOwningPlayer(),
    EncyclopediaPageTextClass)->Init(text);
    return AddPage(category, title, page);
}

UEncyclopediaEntry* UEncyclopediaUI::AddPage(const UEncyclopediaCategory* category, const FText& title, UEncyclopediaPage* page) {    
    const auto entry = category->AddEntry(title, page);
    Entries.Add(title.ToString(), entry);
    return entry;
}

void UEncyclopediaUI::Fill(UEncyclopedia* encyclopedia, TArray<TPair<FText, FText>>& additionalPages) {
    check(encyclopedia);
    Encyclopedia = encyclopedia;
    Categories->ClearChildren();

    {
        const auto settings = AddCategory(TEXT("Settings"));

        const auto cheatsPage = CreateWidget<UEncyclopediaPageCheats>(GetOwningPlayer(), UEncyclopediaPageCheatsClass)->Init();
        AddPage(settings, FText::FromString(TEXT("Cheats")), cheatsPage);
    }

    {
        const auto concepts = AddCategory(TEXT("Concepts"));
        for (const auto& page : additionalPages)
            CreateAndAddTextPage(concepts, page.Key, page.Value);
    }

    {
        const auto resources = AddCategory(TEXT("Resources"));

        const auto rawMaterialsSet = encyclopedia->FindRawMaterials();
        const auto constructionResourcesSet = encyclopedia->FindConstructionResources();
        const auto goodsSet = encyclopedia->FindGoods();
        const auto needsSet = encyclopedia->FindNeeds();

        TArray<UResource*> intermediateProducts;
        intermediateProducts.Append(encyclopedia->GetAllResources());
        intermediateProducts.RemoveAllSwap([&rawMaterialsSet](const UResource* it){return rawMaterialsSet.Contains(it);});
        intermediateProducts.RemoveAllSwap([&constructionResourcesSet](const UResource* it){return constructionResourcesSet.Contains(it);});
        intermediateProducts.RemoveAllSwap([&goodsSet](const UResource* it){return goodsSet.Contains(it);});
        intermediateProducts.RemoveAllSwap([&needsSet](const UResource* it){return needsSet.Contains(it);});
        intermediateProducts.RemoveSwap(encyclopedia->People);
        intermediateProducts.RemoveSwap(encyclopedia->Workforce);
        
        TArray<UResource*> rawMaterials;    
        for (const auto resource : rawMaterialsSet)
            rawMaterials.Add(resource);
        TArray<UResource*> constructionResources;
        for (const auto resource : constructionResourcesSet)
            constructionResources.Add(resource);
        TArray<UResource*> goods;
        for (const auto resource : goodsSet)
            goods.Add(resource);
        TArray<UResource*> needs;
        for (const auto resource : needsSet)
            needs.Add(resource);

        rawMaterials.Sort();
        constructionResources.Sort();
        goods.Sort();
        needs.Sort();
        intermediateProducts.Sort();

        const auto rawCategory = resources->AddSubCategory(TEXT("Raw Materials"));
        for (const auto resource : rawMaterials)
            resource->EncyclopediaEntry = CreateAndAddResourcePage(rawCategory, resource);
        
        const auto intermediateCategory = resources->AddSubCategory(TEXT("Intermediate Products"));
        for (const auto resource : intermediateProducts)
            resource->EncyclopediaEntry = CreateAndAddResourcePage(intermediateCategory, resource);
        
        const auto constructionCategory = resources->AddSubCategory(TEXT("Construction Materials"));
        for (const auto resource : constructionResources)
            resource->EncyclopediaEntry = CreateAndAddResourcePage(constructionCategory, resource);

        const auto goodsCategory = resources->AddSubCategory(TEXT("Goods"));
        for (const auto resource : goods)
            resource->EncyclopediaEntry = CreateAndAddResourcePage(goodsCategory, resource);

        const auto needsCategory = resources->AddSubCategory(TEXT("Needs"));
        for (const auto resource : needs)
            resource->EncyclopediaEntry = CreateAndAddResourcePage(needsCategory, resource);
    }

    {
        const auto biomes = AddCategory(TEXT("Biomes"));
        
        const auto naturalResources = biomes->AddSubCategory(TEXT("Natural Resources"));
        for (const auto naturalResource : encyclopedia->GetAllNaturalResources())
            naturalResource->EncyclopediaEntry = CreateAndAddNaturalResourcePage(naturalResources, naturalResource);
    }

    {
        const auto people = AddCategory(TEXT("People"));

        const auto needs = people->AddSubCategory(TEXT("Needs"));
        for (const auto need : encyclopedia->GetAllNeeds())
            need->EncyclopediaEntry = CreateAndAddNeedPage(needs, need);

        const auto policies = people->AddSubCategory(TEXT("Policies"));
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
                if (building == encyclopedia->Conveyor) { // special handling for conveyor page
                    const auto conveyorPage = CreateWidget<UEncyclopediaPageBuilding>(
                        GetOwningPlayer(),
                        EncyclopediaPageBuildingClass)->InitConveyor(Encyclopedia);

                    const auto conveyorEntry = AddPage(category, building->Name, conveyorPage);            
                    encyclopedia->Conveyor->EncyclopediaEntry = conveyorEntry;
                    encyclopedia->ConveyorNode->EncyclopediaEntry = conveyorEntry;
                    encyclopedia->ConveyorLink->EncyclopediaEntry = conveyorEntry;
                } else
                    building->EncyclopediaEntry = CreateAndAddBuildingPage(category, building);
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
    UEncyclopediaCategory* currentCat = UX::GetParentWidget<UEncyclopediaCategory>(entry);
    while(currentCat) {
        currentCat->CollapseAll(oldCat, false);
        currentCat->ExpandableArea->SetIsExpanded_Animated(true);
        oldCat = currentCat;
        currentCat = UX::GetParentWidget<UEncyclopediaCategory>(currentCat);
    }
    CollapseAll(oldCat);

    OpenedEntry = entry;
}

void UEncyclopediaUI::OpenPageByName(FText name) {
    if (const auto entry = FindPage(name))
        OpenPage(entry);
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

UEncyclopediaEntry* UEncyclopediaUI::FindPage(const FText& name) const {
    if (const auto entry = Entries.Find(name.ToString()))
        return *entry;
    return nullptr;
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

UEncyclopediaEntry* UEncyclopediaCategory::AddEntry(const FText& title, UEncyclopediaPage* page) const {
    const auto encyclopedia = UX::GetParentWidget<UEncyclopediaUI>(this);
    const auto entry = CreateWidget<UEncyclopediaEntry>(
        GetOwningPlayer(),
        encyclopedia->EncyclopediaEntryClass)->Init(title, page);
    Entries->AddChild(entry);
    return entry;
}

UEncyclopediaCategory* UEncyclopediaCategory::AddSubCategory(const FString& name) const {
    const auto encyclopedia = UX::GetParentWidget<UEncyclopediaUI>(this);
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
    UX::GetParentWidget<UEncyclopediaUI>(this)->OpenPage(this);
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
