// Fill out your copyright notice in the Description page of Project Settings.


#include "XActor.h"

UGameInstanceX* AXActor::GetGameInstance() const {
    return GetWorld()->GetGameInstance<UGameInstanceX>();
}
