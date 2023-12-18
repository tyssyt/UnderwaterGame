// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaLinkRichTextBlockDecorator.h"

#include "The.h"
#include "Components/RichTextBlock.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Widgets/Input/SRichTextHyperlink.h"
#include "XD/BlueprintHolder.h"

FRichInlineEncyclopediaLinkDecorator::FRichInlineEncyclopediaLinkDecorator(URichTextBlock* inOwner, const UEncyclopediaLinkRichTextBlockDecorator* decorator) : FRichTextDecorator(inOwner), LinkStyle(decorator->Style) {}

bool FRichInlineEncyclopediaLinkDecorator::Supports(const FTextRunParseResults& runParseResult, const FString& text) const {
    return runParseResult.Name == TEXT("link");
}

TSharedPtr<SWidget> FRichInlineEncyclopediaLinkDecorator::CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const {
    const TSharedPtr<FSlateHyperlinkRun::FWidgetViewModel> model = MakeShareable(new FSlateHyperlinkRun::FWidgetViewModel);

    const auto encyclopediaUI = The::BPHolder(Owner->GetOwningPlayer())->EncyclopediaUI;
    const auto content = RunInfo.Content;

    FSimpleDelegate delegate;
    delegate.BindLambda([encyclopediaUI, content]() {
        encyclopediaUI->OpenPageByName(content);
    });
    
    return SNew(SRichTextHyperlink, model.ToSharedRef())
        .Text(content)
        .Style(&LinkStyle)
        .OnNavigate(delegate);
}



UEncyclopediaLinkRichTextBlockDecorator::UEncyclopediaLinkRichTextBlockDecorator(const FObjectInitializer& objectInitializer) : Super(objectInitializer) {}

TSharedPtr<ITextDecorator> UEncyclopediaLinkRichTextBlockDecorator::CreateDecorator(URichTextBlock* inOwner) {
    return MakeShareable(new FRichInlineEncyclopediaLinkDecorator(inOwner, this));
}
