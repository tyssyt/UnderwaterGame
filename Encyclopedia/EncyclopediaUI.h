// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Encyclopedia.h"
#include "EncyclopediaPageBuilding.h"
#include "EncyclopediaPageNaturalResource.h"
#include "EncyclopediaPageResource.h"
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
    UEncyclopediaPageResource* CreateResourcePage(UResource* resource) const;
    UEncyclopediaPageNaturalResource* CreateNaturalResourcePage(UNaturalResource* naturalResource) const;
    UEncyclopediaPageBuilding* CreateBuildingPage(UConstructionPlan* building) const;

protected:
    UPROPERTY(BlueprintReadOnly)
    UEncyclopedia* Encyclopedia;
    UPROPERTY(BlueprintReadOnly)
    UEncyclopediaEntry* OpenedEntry;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Title;  

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UScrollBox* Categories;

public:
    void Fill(UEncyclopedia* encyclopedia);

    UFUNCTION(BlueprintCallable)
    void Open();
    UFUNCTION(BlueprintCallable)
    void Close();

    UFUNCTION(BlueprintCallable)
    void OpenPage(UEncyclopediaEntry* entry);
    void ClosePage();

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

    UEncyclopediaEntry* AddEntry(const FString& title, UEncyclopediaPage* page) const;
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
