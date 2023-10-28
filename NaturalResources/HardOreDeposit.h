// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaturalResource.h"
#include "HardOreDeposit.generated.h"

UCLASS()
class XD_API AHardOreDeposit : public ANaturalResourceActor {
    GENERATED_BODY()

public:
    AHardOreDeposit();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
