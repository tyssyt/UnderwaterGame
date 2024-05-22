// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UElectricComponent;
class ASubstation;

class XD_API ElectricityNetwork {

public:
    static constexpr double MAX_WIRE_DISTANCE = 500.f;

    explicit ElectricityNetwork(ASubstation* substation);
    ~ElectricityNetwork();

    TArray<ASubstation*> Substations;

    /*
    * Calculates total Production and Consumption
    * Will shutdown Consumers if Production is not high enough
    */
    void RecomputeStats();

    void CheckForNetworkSplit();

    void MergeNetwork(const ElectricityNetwork* otherNetwork);
    void MergeNetworkNoRecompute(const ElectricityNetwork* otherNetwork);

    int GetTotalConstantProduction() const { return TotalConstantProduction; }
    int GetTotalConstantConsumption() const { return TotalConstantConsumption; }
    
    ASubstation* FindNearestSubstation(const FVector& location);

protected:

    int TotalConstantProduction;
    int TotalConstantConsumption;
    
    bool CollectStats(UElectricComponent* elec);
    TArray<UElectricComponent*> CollectStats();
    auto UnpowerBuildings(TArray<UElectricComponent*>& unpowered) -> void;

};
