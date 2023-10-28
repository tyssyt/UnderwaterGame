// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaturalResource.h"
#include "OilFish.generated.h"

UCLASS()
class XD_API AOilFish : public ANaturalResourceActor {
    GENERATED_BODY()

public:
    AOilFish();

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;
};
