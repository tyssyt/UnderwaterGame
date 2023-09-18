// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

class UElectricComponent;
class ASubstation;

class XD_API ElectricityNetwork {

public:
    inline static constexpr double MAX_WIRE_DISTANCE = 500.f;

    explicit ElectricityNetwork(ASubstation* substation);
    ~ElectricityNetwork();

    TArray<ASubstation*> substations;

    /*
    * Calculates total Production and Consumption
    * Will shutdown Consumers if Production is not high enough
    */
    void RecomputeStats();

    void CheckForNetworkSplit();

    void MergeNetwork(ElectricityNetwork* otherNetwork);
    void MergeNetworkNoRecompute(ElectricityNetwork* otherNetwork);

    int GetTotalConstantProduction() const {return TotalConstantProduction;}
    int GetTotalConstantConsumption() const {return TotalConstantConsumption;}

protected:

    int TotalConstantProduction;
    int TotalConstantConsumption;
    
    std::vector<UElectricComponent*> CollectStats();
    auto UnpowerBuildings(std::vector<UElectricComponent*>& unpowered) -> void;

};
