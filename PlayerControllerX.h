// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BlueprintHolder.h"
#include "SelectedUI.h"
#include "Buildings/Building.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerX.generated.h"

UCLASS(Abstract)
class XD_API APlayerControllerX : public APlayerController {
    GENERATED_BODY()

    USelectedUI* SelectedUI;

public:
    APlayerControllerX();

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBlueprintHolder> BlueprintHolderClass;

    UPROPERTY()
    UBlueprintHolder* BlueprintHolder;

    virtual void BeginPlay() override;
    virtual void AcknowledgePossession(APawn* p) override;

    void TickUI();

    void ShowMouseCursor(bool isControlledByMouse);
    void UpdateSelected(AActor* actor);
    void Deselect();

    template <class T> T* GetUnderCursor() {
        FHitResult hitResult;
        if (GetUnderCursor(&hitResult))
            return Cast<T>(hitResult.Actor.Get());
        return nullptr;
    }

    bool GetUnderCursor(FHitResult*);


};
