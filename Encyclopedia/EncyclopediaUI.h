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

    UEncyclopediaPage* CreatePage(UObject* content) const;
    UEncyclopediaCategory* AddCategory(const FString& name) const;
    UEncyclopediaEntry* AddEntry(const UEncyclopediaCategory* category, const FText& title, UObject* content);

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
    UEncyclopediaEntry* GetOpenedEntry() const { return IsVisible() ? OpenedEntry : nullptr; }
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
    UFUNCTION(BlueprintCallable)
    void RefreshPage();

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

    UEncyclopediaEntry* AddEntry(const FText& title, UObject* content) const;
    UEncyclopediaCategory* AddSubCategory(const FString& name) const;

    void CollapseAll(UEncyclopediaCategory* ignore = nullptr, bool includeSelf = true) const;
};

UCLASS(Abstract)
class XD_API UEncyclopediaEntry : public UUserWidget {
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly)
    bool Opened;

    UPROPERTY(BlueprintReadOnly)
    UObject* Content;

    UPROPERTY(BlueprintReadWrite)
    FSlateColor HighlightColor;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* Title;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* Button;

public:
    UEncyclopediaEntry* Init(const FText& label, UObject* content);

    FText GetTitle() const { return Title->GetText(); }
    UObject* GetContent() const { return Content; }

    UFUNCTION(BlueprintCallable)
    void SetHighlight(bool highlight) const;

    UFUNCTION(BlueprintCallable)
    void OnClicked();

    void SetOpen(bool open);

    void Update();
};

UCLASS()
class XD_API UTextContent : public UObject {
    GENERATED_BODY()
public:
    FText Text;
};
