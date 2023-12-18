// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerOverlayUI.h"

#include "PowerOverlay.h"

EPowerOverlayMode UPowerOverlayUI::GetMode() const {
    if (CheckBoxTogglePower->GetCheckedState() == ECheckBoxState::Checked)
        return EPowerOverlayMode::TogglePower;
    if (CheckBoxConnect->GetCheckedState() == ECheckBoxState::Checked)
        return EPowerOverlayMode::Connect;
    if (CheckBoxDisconnect->GetCheckedState() == ECheckBoxState::Checked)
        return EPowerOverlayMode::Disconnect;
    return EPowerOverlayMode::None;
}

void UPowerOverlayUI::Reset() const {
    CheckBoxTogglePower->SetCheckedState(ECheckBoxState::Unchecked);
    CheckBoxConnect->SetCheckedState(ECheckBoxState::Unchecked);
    CheckBoxDisconnect->SetCheckedState(ECheckBoxState::Unchecked);
}