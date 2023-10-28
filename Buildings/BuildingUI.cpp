// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingUI.h"

UBuildingUI* UBuildingUI::Init(UConstructionPlan* building) {
    SetToolTipText(building->Name);
    Image->SetBrushFromTexture(building->Image);    
    Building = building;
    return this;
}

UConstructionPlan* UBuildingUI::GetBuilding() const {
    return Building;
}
