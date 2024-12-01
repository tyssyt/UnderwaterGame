// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaUI.h"

#include "Collections.h"
#include "EncyclopediaPageNeed.h"
#include "EncyclopediaPageText.h"
#include "The.h"
#include "UI.h"
#include "XD/Cheats.h"
#include "XD/Buildings/ConstructionPlan.h"
#include "XD/Resources/Resource.h"

UEncyclopediaPage* UEncyclopediaUI::CreatePage(UObject* content) const {
    if (const auto resource = Cast<UResource>(content))
        return CreateWidget<UEncyclopediaPageResource>(GetOwningPlayer(),EncyclopediaPageResourceClass)
            ->Init(resource, Encyclopedia);
    
    if (const auto naturalResource = Cast<UNaturalResource>(content))
        return CreateWidget<UEncyclopediaPageNaturalResource>(GetOwningPlayer(), EncyclopediaPageNaturalResourceClass)
            ->Init(naturalResource, Encyclopedia);

    if (const auto building = Cast<UConstructionPlan>(content)) {
        const auto page = CreateWidget<UEncyclopediaPageBuilding>(GetOwningPlayer(), EncyclopediaPageBuildingClass);
        if (building == Encyclopedia->Conveyor)
            return page->InitConveyor(Encyclopedia);
        return page->Init(building, Encyclopedia);
    }

    if (const auto need = Cast<UNeed>(content))
        return CreateWidget<UEncyclopediaPageNeed>(GetOwningPlayer(), EncyclopediaPageNeedClass)
            ->Init(need, Encyclopedia);
    
    if (const auto event = Cast<UEvent>(content))
        return CreateWidget<UEncyclopediaPageEvent>(GetOwningPlayer(), EncyclopediaPageEventClass)
            ->Init(event);

    if (const auto text = Cast<UTextContent>(content))
        return CreateWidget<UEncyclopediaPageText>(GetOwningPlayer(), EncyclopediaPageTextClass)
            ->Init(text->Text);

    if (const auto cheats = Cast<UCheats>(content))
        return CreateWidget<UEncyclopediaPageCheats>(GetOwningPlayer(), UEncyclopediaPageCheatsClass)->Init();

    checkNoEntry();
    return nullptr;
}

UEncyclopediaCategory* UEncyclopediaUI::AddCategory(const FString& name) const {
    const auto category = CreateWidget<UEncyclopediaCategory>(
        GetOwningPlayer(),
        EncyclopediaCategoryClass)->Init(FText::FromString(name));
    Categories->AddChild(category);
    return category;
}

UEncyclopediaEntry* UEncyclopediaUI::AddEntry(const UEncyclopediaCategory* category, const FText& title, UObject* content) {    
    const auto entry = category->AddEntry(title, content);
    Entries.Add(title.ToString(), entry);
    return entry;
}

void UEncyclopediaUI::Fill(UEncyclopedia* encyclopedia, TArray<TPair<FText, FText>>& additionalPages) {
    check(encyclopedia);
    Encyclopedia = encyclopedia;
    Categories->ClearChildren();

    {
        const auto settings = AddCategory(TEXT("Settings"));
        AddEntry(settings, FText::FromString(TEXT("Cheats")), The::Cheats(this));
    }

    {
        const auto concepts = AddCategory(TEXT("Concepts"));
        for (const auto& page : additionalPages) {
            const auto content = NewObject<UTextContent>(this);
            content->Text = page.Value;
            AddEntry(concepts, page.Key, content);
        }
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
            resource->EncyclopediaEntry = AddEntry(rawCategory, resource->Name, resource);
        
        const auto intermediateCategory = resources->AddSubCategory(TEXT("Intermediate Products"));
        for (const auto resource : intermediateProducts)
            resource->EncyclopediaEntry = AddEntry(intermediateCategory, resource->Name, resource);
        
        const auto constructionCategory = resources->AddSubCategory(TEXT("Construction Materials"));
        for (const auto resource : constructionResources)
            resource->EncyclopediaEntry = AddEntry(constructionCategory, resource->Name, resource);

        const auto goodsCategory = resources->AddSubCategory(TEXT("Goods"));
        for (const auto resource : goods)
            resource->EncyclopediaEntry = AddEntry(goodsCategory, resource->Name, resource);

        const auto needsCategory = resources->AddSubCategory(TEXT("Needs"));
        for (const auto resource : needs)
            resource->EncyclopediaEntry = AddEntry(needsCategory, resource->Name, resource);
    }

    {
        const auto biomes = AddCategory(TEXT("Biomes"));
        
        const auto naturalResources = biomes->AddSubCategory(TEXT("Natural Resources"));
        for (const auto naturalResource : encyclopedia->GetAllNaturalResources())
            naturalResource->EncyclopediaEntry = AddEntry(naturalResources, naturalResource->Name, naturalResource);
    }

    {
        const auto people = AddCategory(TEXT("People"));

        const auto needs = people->AddSubCategory(TEXT("Needs"));
        for (const auto need : encyclopedia->GetAllNeeds())
            need->EncyclopediaEntry = AddEntry(needs, need->Name, need);

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
                    const auto conveyorEntry = AddEntry(category, building->Name, building);            
                    encyclopedia->Conveyor->EncyclopediaEntry = conveyorEntry;
                    encyclopedia->ConveyorNode->EncyclopediaEntry = conveyorEntry;
                    encyclopedia->ConveyorLink->EncyclopediaEntry = conveyorEntry;
                } else
                    building->EncyclopediaEntry = AddEntry(category, building->Name, building);
            }
        }
    }

    {
        const auto events = AddCategory(TEXT("Events"));        
        for (const auto event : encyclopedia->GetAllEvents())
            event->EncyclopediaEntry = AddEntry(events, event->Name, event);
    }

    CollapseAll();
}

void UEncyclopediaUI::Open() {
    AddToViewport();
    GetOwningPlayer()->InputComponent->BindAction("Deselect", IE_Pressed, this, &UEncyclopediaUI::Close);
    RefreshPage();
}
void UEncyclopediaUI::Close() {
    RemoveFromParent();
    GetOwningPlayer()->InputComponent->RemoveActionBinding("Deselect", IE_Pressed);
}

void UEncyclopediaUI::OpenPage(UEncyclopediaEntry* entry) {
    if (entry != OpenedEntry)
        ClosePage();

    if (!IsVisible())
        Open();

    if (!entry || entry == OpenedEntry)
        return;

    OpenedEntry = entry;
    entry->SetOpen(true);
    Title->SetText(entry->GetTitle());
    SetContentForSlot(TEXT("Page"), CreatePage(entry->GetContent()));

    UEncyclopediaCategory* oldCat = nullptr;
    UEncyclopediaCategory* currentCat = UX::GetParentWidget<UEncyclopediaCategory>(entry);
    while(currentCat) {
        currentCat->CollapseAll(oldCat, false);
        currentCat->ExpandableArea->SetIsExpanded_Animated(true);
        oldCat = currentCat;
        currentCat = UX::GetParentWidget<UEncyclopediaCategory>(currentCat);
    }
    CollapseAll(oldCat);
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

void UEncyclopediaUI::RefreshPage() {
    if (OpenedEntry)
        SetContentForSlot(TEXT("Page"), CreatePage(OpenedEntry->GetContent()));
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

UEncyclopediaEntry* UEncyclopediaCategory::AddEntry(const FText& title, UObject* content) const {
    const auto encyclopedia = UX::GetParentWidget<UEncyclopediaUI>(this);
    const auto entry = CreateWidget<UEncyclopediaEntry>(
        GetOwningPlayer(),
        encyclopedia->EncyclopediaEntryClass)->Init(title, content);
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

UEncyclopediaEntry* UEncyclopediaEntry::Init(const FText& label, UObject* content) {
    Title->SetText(label);
    Content = content;
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

void UEncyclopediaEntry::Update() {
    const auto encyclopedia = UX::GetParentWidget<UEncyclopediaUI>(this);
    if (encyclopedia->GetOpenedEntry() == this) {
        encyclopedia->RefreshPage();
    }
}
