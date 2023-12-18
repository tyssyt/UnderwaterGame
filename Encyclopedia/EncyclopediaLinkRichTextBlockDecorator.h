// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "EncyclopediaLinkRichTextBlockDecorator.generated.h"

class UEncyclopediaLinkRichTextBlockDecorator;

class FRichInlineEncyclopediaLinkDecorator : public FRichTextDecorator {
public:
    FRichInlineEncyclopediaLinkDecorator(URichTextBlock* inOwner, const UEncyclopediaLinkRichTextBlockDecorator* Decorator);
    virtual bool Supports(const FTextRunParseResults& runParseResult, const FString& text) const override;

protected:
    virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& runInfo, const FTextBlockStyle& textStyle) const override;
    FHyperlinkStyle LinkStyle;
};

UCLASS(Abstract)
class XD_API UEncyclopediaLinkRichTextBlockDecorator : public URichTextBlockDecorator {
    GENERATED_BODY()

public:
    UEncyclopediaLinkRichTextBlockDecorator(const FObjectInitializer& objectInitializer);

    virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* inOwner) override;

    UPROPERTY(EditAnywhere, Category=Appearance)
    FHyperlinkStyle Style;
};
