#pragma once

#include "CameraPawn.h"
#include "Buildings/Building.h"
#include "Buildings/IndoorBuilding.h"

#include "UObject/Class.h"

class BuilderMode {
public:
    virtual ~BuilderMode();
    virtual void Tick(const ACameraPawn& camera);
    virtual AActor* Confirm(const ACameraPawn& camera);
    virtual UClass* IDK(); //TODO better name or replace with a better solution
};

class BuildingBuilderMode : public BuilderMode {

    enum Phase { Positioning, Rotating, Done };

    Phase Phase = Positioning;
    ABuilding* const Preview;

    void Position(const ACameraPawn& camera);
    void Rotate(const ACameraPawn& camera);

public:
    BuildingBuilderMode(ABuilding* preview);
    ~BuildingBuilderMode();

    virtual void Tick(const ACameraPawn& camera);
    virtual AActor* Confirm(const ACameraPawn& camera);
    virtual UClass* IDK();
};

class ConveyorBuilderMode : public BuilderMode {

    ABuilding* Source = nullptr;

public:
    ConveyorBuilderMode();

    virtual void Tick(const ACameraPawn& camera);
    virtual AActor* Confirm(const ACameraPawn& camera);
    virtual UClass* IDK();
};

class IndoorBuilderMode : public BuilderMode {

    bool Buildable = false;
    AIndoorBuilding* Preview;

    UMaterial* OldMaterial;

public:
    IndoorBuilderMode(AIndoorBuilding* preview);
    ~IndoorBuilderMode();

    virtual void Tick(const ACameraPawn& camera);
    virtual AActor* Confirm(const ACameraPawn& camera);
    virtual UClass* IDK();

private:
    void setInvisible();
    void setNotBuildable(UMaterial* RedGhostMaterial);
    void setBuildable();

};
