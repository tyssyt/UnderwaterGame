// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ResourceConsumption.h"

class AHabitat;

class XD_API HabitatPopulationManager {

public:
    HabitatPopulationManager(AHabitat* habitat);
    ~HabitatPopulationManager();

    void NotifyBuildingsChanged();
    void TickPopulation();

    int GetMaxPop() const;
    int GetSettledPop() const; // Fed + Starving
    
    int GetFedPop() const;
    int GetStarvingPop() const;
    int GetHomelessPop() const;

    int GetWorkforce() const;

    // a vec with necessary resource consumption. Add food in the constructor

private:
    int Tick;
    bool BuildingsChangedNotificationRecieved;
    AHabitat* Habitat;

    int MaxPop;
    int SettledPop;
    int StarvingPop;
    int HomelessPop;

    int Workforce;

    ResourceConsumption Hunger;

    void UpdateMaxPop();
    bool GrowOrShrinkPop();
    int ComputeWorkforce() const;
    void RelocatePop() const;


};
