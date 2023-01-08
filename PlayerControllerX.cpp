// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerX.h"

APlayerControllerX::APlayerControllerX() {}

void APlayerControllerX::BeginPlay() {
    Super::BeginPlay();

    BlueprintHolder = NewObject<UBlueprintHolder>(this, BlueprintHolderClass);
    BlueprintHolder->Init(this);
    BlueprintHolder->MainUI->AddToViewport();
}

void APlayerControllerX::AcknowledgePossession(APawn* p) {
    Super::AcknowledgePossession(p);
    ShowMouseCursor(true);
}

void APlayerControllerX::ShowMouseCursor(bool showMouseCursor) {
    bShowMouseCursor = showMouseCursor;
    bEnableClickEvents = showMouseCursor;
    bEnableMouseOverEvents = showMouseCursor;
    SetIgnoreLookInput(showMouseCursor);

    if (showMouseCursor)
        SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
    else
        SetInputMode(FInputModeGameOnly());
}

void APlayerControllerX::TickUI() {
    if (SelectedUI)
        SelectedUI->Tick();
}

void APlayerControllerX::UpdateSelected(AActor* actor) {
    USelectedUI* ui = BlueprintHolder->GetUI(actor);
    if (ui && SelectedUI != ui) {
        SelectedUI = ui;
        //		ui->Tick();
        BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), ui);
    }
}

void APlayerControllerX::Deselect() {
    SelectedUI = nullptr;
    BlueprintHolder->MainUI->SetContentForSlot(TEXT("Selection"), nullptr);
}

bool APlayerControllerX::GetUnderCursor(FHitResult* hitResult) {
    if (!bShowMouseCursor)
        return false;

    float mouseX, mouseY;
    if (GetMousePosition(mouseX, mouseY) && GetHitResultAtScreenPosition(FVector2D(mouseX, mouseY), ECollisionChannel::ECC_Visibility, true, *hitResult) && hitResult->Actor.IsValid())
        return true;

    return false;
}
