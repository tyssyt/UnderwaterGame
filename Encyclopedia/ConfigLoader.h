// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Encyclopedia.h"
#include "XD/Hotbar/HotbarDock.h"

DECLARE_LOG_CATEGORY_EXTERN(LogConfigLoader, Log, All);

struct ConfigLoader { // TODO could just be a namespace, but that breaks the build somehow -.- <3 unreal
    static TPair<UEncyclopedia*, TArray<TPair<FText, FText>>> Load(const UHotbarDock* dock, UObject* Outer);
};
