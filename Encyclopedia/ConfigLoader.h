// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Encyclopedia.h"
#include "XD/Hotbar/HotbarDock.h"

DECLARE_LOG_CATEGORY_EXTERN(LogConfigLoader, Log, All);

namespace ConfigLoader {
    static UEncyclopedia* Load(const UHotbarDock* dock);
};
