#pragma once

#include "Cheats.generated.h"

UCLASS()
class XD_API UCheats : public UObject { // TODO look into the CheatManager and other stuff that comes with unreal
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    // TODO go through all electric construction sites and finish them when enabled
    bool InstantBuild = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    // TODO go through all electric components and reapply conditions when enabled
    bool AlwaysPowered = false;
};
