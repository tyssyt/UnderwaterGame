// Fill out your copyright notice in the Description page of Project Settings.

#include "ComponentX.h"

UComponentX* UComponentX::Init(UComponentInfo* componentInfo) {
    ComponentInfo = componentInfo;
    return this;
}
