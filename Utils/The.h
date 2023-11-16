#pragma once

#include "XD/GameInstanceX.h"

class UBlueprintHolder;
class ACameraPawn;
class APlayerControllerX;
class UElectricityManager;
class UConstructionManager;
class UEncyclopedia;

namespace The {
    template <class T>
    static UEncyclopedia* Encyclopedia(T HasWorld) {
        return HasWorld->GetWorld()->template GetGameInstance<UGameInstanceX>()->TheEncyclopedia;
    }
    template <class T>
    static UConstructionManager* ConstructionManager(T HasWorld) {
        return HasWorld->GetWorld()->template GetGameInstance<UGameInstanceX>()->TheConstructionManager;
    }
    template <class T>
    static UElectricityManager* ElectricityManager(T HasWorld) {
        return HasWorld->GetWorld()->template GetGameInstance<UGameInstanceX>()->TheElectricityManager;
    }
    template <class T>
    static APlayerControllerX* PlayerController(T HasWorld) {
        return HasWorld->GetWorld()->template GetFirstPlayerController<APlayerControllerX>();
    }
    template <class T>
    static ACameraPawn* CameraPawn(T HasWorld) {
        return HasWorld->GetWorld()->template GetFirstPlayerController<APlayerControllerX>()->template GetPawn<ACameraPawn>();
    }
    template <class T>
    static UBlueprintHolder* BPHolder(T HasWorld) {
        return HasWorld->GetWorld()->template GetFirstPlayerController<APlayerControllerX>()->BlueprintHolder;
    }
}
