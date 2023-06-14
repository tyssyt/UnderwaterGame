// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Resources/ResourceBook.h"
#include "Recipes/RecipeBook.h"
#include "Buildings/BuildingBook.h"
#include "Construction/ConstructionManager.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstanceX.generated.h"

/**
 * 
 */
UCLASS()
class XD_API UGameInstanceX : public UGameInstance {
    GENERATED_BODY()

    virtual void Init() override;

public:
    UGameInstanceX();

    // TODO we could also consider using subsystems for some of the below things
    // https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/Subsystems/

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UResourceBook* TheResourceBook;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    URecipeBook* TheRecipeBook;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UBuildingBook* TheBuildingBook;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UConstructionManager* TheConstructionManager;

};
