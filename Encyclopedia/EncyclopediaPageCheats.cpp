// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaPageCheats.h"

#include "The.h"
#include "XD/Cheats.h"

UEncyclopediaPageCheats* UEncyclopediaPageCheats::Init() {
    InstantBuild->SetCheckedState(The::Cheats(this)->InstantBuild ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    InstantBuild->OnCheckStateChanged.AddDynamic(this, &UEncyclopediaPageCheats::UEncyclopediaPageCheats::OnInstantBuildChanged);

    AlwaysPowered->SetCheckedState(The::Cheats(this)->AlwaysPowered ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    AlwaysPowered->OnCheckStateChanged.AddDynamic(this, &UEncyclopediaPageCheats::UEncyclopediaPageCheats::OnAlwaysPoweredChanged);

    return this;
}

void UEncyclopediaPageCheats::OnInstantBuildChanged(bool bIsChecked) {
    UE_LOG(LogTemp, Display, TEXT("Cheat Instant Build: %s"), ( bIsChecked ? TEXT("on") : TEXT("off")));
    The::Cheats(this)->InstantBuild = bIsChecked;
}
void UEncyclopediaPageCheats::OnAlwaysPoweredChanged(bool bIsChecked) {    
    UE_LOG(LogTemp, Display, TEXT("Cheat Always Powered: %s"), ( bIsChecked ? TEXT("on") : TEXT("off")));
    The::Cheats(this)->AlwaysPowered = bIsChecked;
}
