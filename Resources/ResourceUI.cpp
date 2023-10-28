// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceUI.h"

UResourceUI* UResourceUI::Init(UResource* resource) {
    SetToolTipText(resource->Name);
    Image->SetBrushFromTexture(resource->Image);    
    Resource = resource;
    return this;
}
