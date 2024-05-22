#pragma once

#include "CoreMinimal.h"
#include "ConstructionUI.h"
#include "UObject/Object.h"
#include "XD/CameraPawn.h"
#include "XD/Buildings/Highlighted.h"
#include "BuilderMode.generated.h"

class UBuilderModeExtension;

UCLASS()
class UInBuilderMode : public UCondition {
    GENERATED_BODY()

public:
    UInBuilderMode();
};

UCLASS(Abstract)
class XD_API UBuilderMode : public UObject {
    GENERATED_BODY()

protected:
    UPROPERTY()
    UConstructionUI* ConstructionUI;

    UPROPERTY()
    UHighlighted* HighlightedValid;
    UPROPERTY()
    UHighlighted* HighlightedInvalid;
    UPROPERTY()
    UHighlighted* HighlightedOption;

    void PreInit();

public:
    virtual bool Tick(const ACameraPawn& camera) {return true;}
    virtual UClass* IDK() {return nullptr;} //TODO better name or replace with a better solution
    virtual void Stop(bool cancelled) {}
};
