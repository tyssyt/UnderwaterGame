// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElectricComponent.h"
#include "PowerUI.h"
#include "WireComponent.h"
#include "Components/CheckBox.h"
#include "Components/WidgetComponent.h"
#include "UObject/Object.h"
#include "XD/Buildings/Building.h"
#include "PowerOverlay.generated.h"


class UWireComponent;
class ABuilding;
class APlayerControllerX;

UCLASS()
class XD_API UPowerOverlay : public UObject, public FTickableGameObject {
    GENERATED_BODY()

public:
    enum class EMode {None, TogglePower, Connect, Disconnect};

private:

    bool Active;

    uint32 LastFrameNumberWeTicked = INDEX_NONE;
    
    UPROPERTY()
    AActor* ComponentHolder;
    UPROPERTY()
    UPowerUI* PowerUI;
    
    UPROPERTY()
    UMaterialInstance* MatPowered;
    UPROPERTY()
    UMaterialInstance* MatUnpowered;
    UPROPERTY()
    UMaterialInstance* MatDeactivated;
    UPROPERTY()
    UMaterialInstance* MatHighlight;

    struct UModeHighlight {
        EMode Mode;
        ABuilding* Current;

        ABuilding* Source; // for Connect
        UWireComponent* Wire; // for Connect & Disconnect
        TArray<UWireComponent*> Wires; // for Connect & Disconnect
    };

    UModeHighlight ModeHighlight;
    void TickTogglePower();
    void TickConnect();
    void TickDisconnect();
    void ConfirmModeHighlight();
    void ResetModeHighlight(bool deactivate);
    void ResetTogglePower();
    void ResetConnect(bool deactivate);
    void ResetDisconnect(bool deactivate);

    static UElectricComponent* CheckElec(const ABuilding* building);
    bool CanBeConnected(ABuilding* one, ABuilding* two, UWireComponent*& oldWire) const;
    static bool CanBeConnected(const ASubstation* one, const ASubstation* two);
    bool CanBeConnected(const ASubstation* substation, const UElectricComponent* elec, UWireComponent*& oldWire) const;

public:

    // TODO have a list of highlight colors and give wires from different networks different colors

    UPowerOverlay();

    virtual void Tick(float DeltaTime) override;

    virtual TStatId GetStatId() const override {
        RETURN_QUICK_DECLARE_CYCLE_STAT( FMyTickableThing, STATGROUP_Tickables );
    }

    bool IsActive() const {return Active;}
    void Toggle();
    void Activate();
    void Deactivate();
    void OnNetworkUpdate();
    void AddDisconnected(const UElectricComponent* building) const;

private:

    void DoActivate();
    void DoDeactivate();

    void AddWires(ASubstation* substation) const;
    void AddFloatingTexts(const ASubstation* substation) const;
    void HighlightBuildings(const ASubstation* substation) const;
    
    void AddFloatingPowerUI(const ASubstation* substation) const;
    void AddFloatingText(const UElectricComponent* building) const;
    void AddFloatingWidget(FVector location, UUserWidget* widget, const APlayerControllerX* playerController) const;
    static void ScaleFloatingWidget(UWidgetComponent* widgetComponent, FVector cameraLocation);
    void Highlight(const UElectricComponent* building) const;
};

UCLASS(Abstract)
class XD_API UPowerOverlayUI : public UUserWidget {
    GENERATED_BODY()
  
protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* CheckBoxTogglePower;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* CheckBoxConnect;
    
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UCheckBox* CheckBoxDisconnect;

public:

    UPowerOverlay::EMode GetMode() const;
    void Reset() const;
};
