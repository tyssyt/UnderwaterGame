#pragma once

#include "CameraPawn.h"
#include "ConstructionSite.h"
#include "Buildings/Building.h"
#include "Buildings/ConstructionPlan.h"
#include "Buildings/IndoorBuilding.h"

#include "UObject/Class.h"

class BuilderMode {
public:
    virtual ~BuilderMode() {}
    virtual void Tick(const ACameraPawn& camera) {}
    virtual ConstructionSite* Confirm(const ACameraPawn& camera) {return nullptr;}
    virtual UClass* IDK() {return nullptr;} //TODO better name or replace with a better solution
};

class BuildingBuilderMode : public BuilderMode {

    enum Phase { Positioning, Rotating, Done };

    Phase Phase = Positioning;
    UConstructionPlan* const ConstructionPlan;
    ABuilding* const Preview;

    void Position(const ACameraPawn& camera) const;
    void Rotate(const ACameraPawn& camera);

public:
    BuildingBuilderMode(UConstructionPlan* constructionPlan, UWorld* world);
    virtual ~BuildingBuilderMode() override;

    virtual void Tick(const ACameraPawn& camera) override;
    virtual ConstructionSite* Confirm(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
};

class ConveyorBuilderMode : public BuilderMode {

    ABuilding* Source = nullptr;

public:
    ConveyorBuilderMode();

    virtual void Tick(const ACameraPawn& camera) override;
    virtual ConstructionSite* Confirm(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;
};

class IndoorBuilderMode : public BuilderMode {

    bool Buildable = false;
    UConstructionPlan* const ConstructionPlan;
    AIndoorBuilding* Preview;

    UMaterial* OldMaterial;

public:
    IndoorBuilderMode(UConstructionPlan* constructionPlan, UWorld* world);
    virtual ~IndoorBuilderMode() override;

    virtual void Tick(const ACameraPawn& camera) override;
    virtual ConstructionSite* Confirm(const ACameraPawn& camera) override;
    virtual UClass* IDK() override;

private:
    void setInvisible();
    void setNotBuildable(UMaterial* RedGhostMaterial);
    void setBuildable();

};
