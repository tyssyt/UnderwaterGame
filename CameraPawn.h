// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "CameraPawn.generated.h"

UCLASS()
class XD_API ACameraPawn : public APawn {
    GENERATED_BODY()


public:
    ACameraPawn();

    UPROPERTY(EditAnywhere)
    UMaterial* RedGhostMaterial;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere)
    class UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere)
    UPawnMovementComponent* MovementComponent;


    class BuilderMode* BuilderMode;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(float val);
    void MoveRight(float val);
    void MoveUp(float val);
    void Turn(float val);
    void LookUp(float val);

    bool IsShowMouseCursor();
    void ShowMouseCursor(bool isControlledByMouse);
    void ShowMouseCursorTrue();
    void ShowMouseCursorFalse();

    void SelectUnderCursor();
    void Deselect();

    void Hotbar1();
    void Hotbar2();
    void Hotbar3();
    void Hotbar4();
    void Hotbar5();
    void Hotbar6();
    void Hotbar7();
    void Hotbar8();
    void Hotbar9();
    void Hotbar0();
};
