// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XD/Buildings/Substation.h"

#include <vector>

class XD_API ElectricityNetwork {

public:
    ElectricityNetwork(ASubstation* substation);
    ~ElectricityNetwork();

    /*
    * Calculates total Production and Consumption
    * Will shutdown Consumers if Production is not high enough
    */
    void recomputeStats();

    void mergeNetworkNoRecompute(ElectricityNetwork* otherNetwork);

    /*
    * Remove a Substation from this Network
    * This might cause the Network to split into multiple disconnected Networks.
    */
    void removeSubstation(ASubstation* substation);


protected:
    std::vector<ASubstation*> substations;

    int TotalConstantProduction;
    int TotalConstantConsumption;

};
