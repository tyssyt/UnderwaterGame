// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ConstructionOptions.generated.h"

UCLASS(Abstract)
class XD_API UConstructionOption : public UObject {
    GENERATED_BODY()
};

UCLASS()
class XD_API UConstructionOptions : public UObject {
    GENERATED_BODY()

public:
    UPROPERTY()
    TMap<UClass*, UConstructionOption*> Options;

    UConstructionOption* Get(const UClass* Class) const;
    template <class T>
    T* Get(const UClass* Class) const {
        return Cast<T>(Get(Class));
    }
};
