// Fill out your copyright notice in the Description page of Project Settings.

#include "NaturalResourceUI.h"

UNaturalResourceUI* UNaturalResourceUI::Init(UNaturalResource* naturalResource) {
    SetToolTipText(naturalResource->Name);
    Image->SetBrushFromTexture(naturalResource->Image);    
    NaturalResource = naturalResource;
    return this;
}
