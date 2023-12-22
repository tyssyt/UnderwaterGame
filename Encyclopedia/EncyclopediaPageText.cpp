// Fill out your copyright notice in the Description page of Project Settings.


#include "EncyclopediaPageText.h"

UEncyclopediaPageText* UEncyclopediaPageText::Init(const FText& text) {
    Description->SetText(text);
    return this;
}
