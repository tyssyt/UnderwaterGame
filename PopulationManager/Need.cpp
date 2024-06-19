// Fill out your copyright notice in the Description page of Project Settings.

#include "Need.h"

#include "NeedSatisfier.h"
#include "The.h"

UNeed* UNeed::Init(
    const FText& name,
    const TCHAR* image,
    const FText& description,
    const int notificationPop,
    const int startPop
) {
    if (name.IsEmptyOrWhitespace()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to Initialize Need, Name is blank: %s"), *name.ToString());
        return nullptr;
    }

    Name = name;
    Image = LoadObject<UTexture2D>(nullptr, image);
    if (!Image)
        UE_LOG(LogTemp, Error, TEXT("Failed to load Texture %s"), image);
    Description = description;

    NotificationPop = notificationPop;
    StartPop = startPop;
    return this;
}

int UNeed::IsSatisfied(const AHabitat* habitat, int population) { // TODO I think it would be worth it to write some tests for this
    int satisfiedPop = 0;
    // TODO we probably want to sort the satisfiers in some way, so that the "easier" ones get checked first?
    for (const auto satisfier : The::Encyclopedia(habitat)->GetNeedSatisfiers(this)) {
        satisfiedPop += satisfier->IsSatisfied(habitat, population-satisfiedPop);
        if (satisfiedPop >= population)
            break;
    }

    return satisfiedPop;
}

bool UNeed::operator<(const UNeed& other) const {
    const int c1 = StartPop - other.StartPop; 
    if (c1 != 0)
        return c1 < 0;

    const int c2 = Name.CompareTo(other.Name);
    if (c2 != 0)
        return c2 < 0;

    checkNoEntry();
    return this - &other < 0;
}
