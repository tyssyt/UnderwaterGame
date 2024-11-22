// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BlueprintHolder.h"
#include "SelectedUI.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerX.generated.h"

#define COLLISION_LANDSCAPE ECC_GameTraceChannel1

UCLASS(Abstract)
class XD_API APlayerControllerX : public APlayerController {
    GENERATED_BODY()

    USelectedUI* SelectedUI;

public:
    APlayerControllerX();

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBlueprintHolder> BlueprintHolderClass;

    UPROPERTY(BlueprintReadOnly)
    UBlueprintHolder* BlueprintHolder;

    virtual void BeginPlay() override;
    virtual void AcknowledgePossession(APawn* p) override;

    void TickUI() const;

    void ShowMouseCursor(bool isControlledByMouse);
    void Select(USelectedUI* ui);
    void UpdateSelected(AXActor* actor);
    void Deselect();

    template <class T> T* GetUnderCursor() const {
        FHitResult hitResult;
        if (GetHitResultUnderCursor(ECC_Visibility, true, hitResult))
            return Cast<T>(hitResult.GetActor());
        return nullptr;
    }

};
