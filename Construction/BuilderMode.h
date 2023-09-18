#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "XD/CameraPawn.h"
#include "BuilderMode.generated.h"

class UBuilderModeExtension;

UCLASS(Abstract)
class XD_API UBuilderMode : public UObject {
    GENERATED_BODY()

public:
    virtual bool Tick(const ACameraPawn& camera) {return true;}
    virtual UClass* IDK() {return nullptr;} //TODO better name or replace with a better solution
    virtual void Stop(bool success) {}
};
