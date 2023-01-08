// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Resources/ResourceBook.h"
#include "Recipes/RecipeBook.h"
#include "ConstructionManager.h"

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

    UPROPERTY(EditAnywhere)
    UResourceBook* TheResourceBook;

    UPROPERTY(EditAnywhere)
    URecipeBook* TheRecipeBook;

    UPROPERTY(EditAnywhere)
    UConstructionManager* TheConstructionManager;

};
