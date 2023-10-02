// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HotbarSlot.h"
#include "Components/Image.h"
#include "HotbarSlotAction.generated.h"

DECLARE_DELEGATE(ActionDelegate)

class UConstructionPlan;

UCLASS(Abstract)
class XD_API UHotbarSlotAction : public UHotbarSlot {
    GENERATED_BODY()

    ActionDelegate Action;

protected:

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* Image;

public:

    template<class FunctorType>
    UHotbarSlotAction* Init(UTexture2D* image, FunctorType&& lambda) {
        Image->SetBrushFromTexture(image);
        Action.BindLambda(lambda);
        return this;
    }

    virtual void Activate() override;
    virtual UImage* GetImage() override;
};
