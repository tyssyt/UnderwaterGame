// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaturalResource.h"
#include "ConductiveOreDeposit.generated.h"

UCLASS()
class XD_API AConductiveOreDeposit : public ANaturalResourceActor {
    GENERATED_BODY()

public:
    AConductiveOreDeposit();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
