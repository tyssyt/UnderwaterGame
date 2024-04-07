// Fill out your copyright notice in the Description page of Project Settings.

#include "NeedUI.h"

UNeedUI* UNeedUI::Init(UNeed* need) {
    SetToolTipText(need->Name);
    Image->SetBrushFromTexture(need->Image);    
    Need = need;
    return this;
}
