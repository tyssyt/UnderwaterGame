// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

class XD_API ConstructionSite {
public:
    ConstructionSite(AActor* building, UMaterial* ghostMaterial);
    ~ConstructionSite();

    void BeginConstruction();

    AActor* Building;
    //class ABuilderShip* BuilderShip;

private:
    UMaterial* OldMaterial;

};
