// Fill out your copyright notice in the Description page of Project Settings.

#include "Need.h"

#include "NeedSatisfier.h"
#include "The.h"

UNeed* UNeed::Init(
    const FText& name,
    const TCHAR* image,
    const FText& description,
    const int notificationPop,
    const int startPop,
    const int fullPop
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
    FullPop = fullPop;
    return this;
}

int UNeed::IsSatisfied(const AHabitat* habitat, int population) { // TODO I think it would be worth it to write some tests for this
    if (population < StartPop)
        return population;

    int adjustedPop;
    if (population < FullPop) {
        const float factor = static_cast<float>(population - StartPop) / (FullPop - StartPop);
        adjustedPop = static_cast<int>(factor * FullPop);
    } else
        adjustedPop = population;
    
    int satisfiedPop = 0;
    // TODO we probably want to sort the satisfiers in some way, so that the "easier" ones get checked first?
    for (const auto satisfier : The::Encyclopedia(habitat)->GetNeedSatisfiers(this)) {
        satisfiedPop += satisfier->IsSatisfied(habitat, adjustedPop-satisfiedPop);
        if (satisfiedPop >= adjustedPop)
            break;
    }

    if (population < FullPop) {
        const float satisfiedPercent = static_cast<float>(satisfiedPop) / FullPop;
        // UE_LOG(LogTemp, Warning, TEXT("Adjusting Pop, Full: %i, Adjusted %i, Satisfied: %i, ReadjustedSatisfied: %i"), population, adjustedPop, satisfiedPop, StartPop + static_cast<int>(satisfiedPercent * (FullPop - StartPop)));
        return StartPop + static_cast<int>(satisfiedPercent * (FullPop - StartPop));
    }
    return satisfiedPop;
}

bool UNeed::operator<(const UNeed& other) const {
    const int c1 = StartPop - other.StartPop; 
    if (c1 != 0)
        return c1 < 0;

    const int c2 = FullPop - other.FullPop; 
    if (c2 != 0)
        return c2 < 0;

    const int c3 = Name.CompareTo(other.Name);
    if (c3 != 0)
        return c2 < 0;

    checkNoEntry();
    return this - &other < 0;
}
