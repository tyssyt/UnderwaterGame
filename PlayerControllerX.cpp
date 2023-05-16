// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerX.h"

#include "GameInstanceX.h"

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

void APlayerControllerX::TickUI() const {
    if (SelectedUI)
        SelectedUI->Tick();

    GetGameInstance<UGameInstanceX>()->TheConstructionManager->UpdateUI(BlueprintHolder->MainUI->WBP_RessourcesUI);
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

bool APlayerControllerX::GetUnderCursor(FHitResult* hitResult) const {
    if (!bShowMouseCursor)
        return false;

    float mouseX, mouseY;
    if (GetMousePosition(mouseX, mouseY) && GetHitResultAtScreenPosition(FVector2D(mouseX, mouseY), ECollisionChannel::ECC_Visibility, true, *hitResult) && hitResult->HasValidHitObjectHandle())
        return true;

    return false;
}
