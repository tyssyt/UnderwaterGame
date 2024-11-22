// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Encyclopedia.h"
#include "EncyclopediaPageBuilding.h"
#include "EncyclopediaPageCheats.h"
#include "EncyclopediaPageEvent.h"
#include "EncyclopediaPageNaturalResource.h"
#include "EncyclopediaPageNeed.h"
#include "EncyclopediaPageResource.h"
#include "EncyclopediaPageText.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ExpandableArea.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "EncyclopediaUI.generated.h"

class UEncyclopediaCategory;
class UEncyclopediaEntry;
class UEncyclopediaPage;

UCLASS(Abstract)
class XD_API UEncyclopediaUI : public UUserWidget {
    GENERATED_BODY()

    UEncyclopediaCategory* AddCategory(const FString& name) const;
    UEncyclopediaEntry* CreateAndAddResourcePage(const UEncyclopediaCategory* category, UResource* resource);
    UEncyclopediaEntry* CreateAndAddNaturalResourcePage(const UEncyclopediaCategory* category, UNaturalResource* naturalResource);
    UEncyclopediaEntry* CreateAndAddBuildingPage(const UEncyclopediaCategory* category, UConstructionPlan* building);
    UEncyclopediaEntry* CreateAndAddNeedPage(const UEncyclopediaCategory* category, UNeed* need);
    UEncyclopediaEntry* CreateAndAddEventPage(const UEncyclopediaCategory* category, UEvent* event);
    UEncyclopediaEntry* CreateAndAddTextPage(const UEncyclopediaCategory* category, const FText& name, const FText& text);
    UEncyclopediaEntry* AddPage(const UEncyclopediaCategory* category, const FText& title, UEncyclopediaPage* page);

protected:
    UPROPERTY()
    TMap<FString, UEncyclopediaEntry*> Entries;
    UPROPERTY(BlueprintReadOnly)
    UEncyclopedia* Encyclopedia;
    UPROPERTY(BlueprintReadOnly)
    UEncyclopediaEntry* OpenedEntry;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Title;  

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UScrollBox* Categories;

public:
    void Fill(UEncyclopedia* encyclopedia, TArray<TPair<FText, FText>>& additionalPages);

    UFUNCTION(BlueprintCallable)
    void Open();
    UFUNCTION(BlueprintCallable)
    void Close();

    UFUNCTION(BlueprintCallable)
    void OpenPage(UEncyclopediaEntry* entry);
    UFUNCTION(BlueprintCallable)
    void OpenPageByName(FText name);
    UFUNCTION(BlueprintCallable)
    void ClosePage();

    UEncyclopediaEntry* FindPage(const FText& name) const;

    UFUNCTION(BlueprintCallable)
    void CollapseAll(UEncyclopediaCategory* ignore = nullptr) const;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaCategory> EncyclopediaCategoryClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaEntry> EncyclopediaEntryClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageResource> EncyclopediaPageResourceClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageNaturalResource> EncyclopediaPageNaturalResourceClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageBuilding> EncyclopediaPageBuildingClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageNeed> EncyclopediaPageNeedClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageEvent> EncyclopediaPageEventClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageText> EncyclopediaPageTextClass;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UEncyclopediaPageCheats> UEncyclopediaPageCheatsClass;
};

UCLASS(Abstract)
class XD_API UEncyclopediaCategory : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Label;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* Entries;

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UExpandableArea* ExpandableArea;

    UEncyclopediaCategory* Init(const FText& label);

    UEncyclopediaEntry* AddEntry(const FText& title, UEncyclopediaPage* page) const;
    UEncyclopediaCategory* AddSubCategory(const FString& name) const;

    void CollapseAll(UEncyclopediaCategory* ignore = nullptr, bool includeSelf = true) const;
};

UCLASS(Abstract)
class XD_API UEncyclopediaEntry : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly)
    bool Opened;

    UPROPERTY(BlueprintReadWrite)
    FSlateColor HighlightColor;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Title;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button;

public:
    UPROPERTY(BlueprintReadWrite)
    UEncyclopediaPage* Page;

    UEncyclopediaEntry* Init(const FText& label, UEncyclopediaPage* page);

    UFUNCTION(BlueprintCallable)
    void SetHighlight(bool highlight) const;

    UFUNCTION(BlueprintCallable)
    void OnClicked();

    void SetOpen(bool open);

    FText GetTitle() const;
};
