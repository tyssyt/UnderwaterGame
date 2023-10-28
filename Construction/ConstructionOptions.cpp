// Fill out your copyright notice in the Description page of Project Settings.

#include "ConstructionOptions.h"

UConstructionOption* UConstructionOptions::Get(const UClass* Class) const {
    if (const auto option = Options.Find(Class))
        return *option;
    return nullptr;
}
