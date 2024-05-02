// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD/PlayerControllerX.h"
#include "ElectricComponent.h"
#include "PowerUI.h"
#include "WireComponent.h"
#include "Components/WidgetComponent.h"
#include "UObject/Object.h"
#include "XD/Buildings/Highlighted.h"
#include "PowerOverlay.generated.h"

class ABuilding;


enum class EPowerOverlayMode {None, TogglePower, Connect, Disconnect};

UCLASS()
class XD_API UPowerOverlay : public UObject, public FTickableGameObject {
    GENERATED_BODY()

protected:
    bool Active;

    uint32 LastFrameNumberWeTicked = INDEX_NONE;
    
    UPROPERTY()
    AActor* ComponentHolder;
    UPROPERTY()
    UPowerUI* PowerUI;

    UPROPERTY()
    UHighlighted* HighlightPowered;
    UPROPERTY()
    UHighlighted* HighlightUnpowered;
    UPROPERTY()
    UHighlighted* HighlightDeactivated;
    UPROPERTY()
    UHighlighted* HighlightUnderCursor;

    struct UModeHighlight {
        EPowerOverlayMode Mode;
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
    void ConfirmTogglePower() const;
    void ConfirmConnect();
    void ConfirmDisconnect() const;
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
    void RemoveHighlight(const UElectricComponent* building);
};