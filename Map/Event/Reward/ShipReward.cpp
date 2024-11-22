// Fill out your copyright notice in the Description page of Project Settings.


#include "ShipReward.h"

#include "The.h"

void UShipReward::Give(UEventInstance* event) {
    The::Map(this)->AddScout(event->Coordinate);
}
