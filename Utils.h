#pragma once
#include "CommonUILibrary.h"

class UElectricityManager;
class UConstructionManager;
class UEncyclopedia;
class UWidget;
class UGameInstanceX;

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
}

namespace UI {
    template <class T>
    static T* GetParentWidget(UWidget* Widget) {
        return Cast<T>(UCommonUILibrary::FindParentWidgetOfType(Widget, T::StaticClass()));
    }
}

namespace Logging {
    static FString ToString(UObject* obj, bool includeEngineClasses = false, EFieldIterationFlags flags = EFieldIterationFlags::None);
}

namespace MultiMaps {
    template <class K, class V>
    static void AddTo(TMap<K, TArray<V>>& map, const K& key, const V& value) {
        map.FindOrAdd(key).Add(value);
    }
    template <class K, class V>
    static void AddTo(TMap<K, TArray<V>>& map, K&& key, const V& value) {
        map.FindOrAdd(MoveTempIfPossible(key)).Add(value);
    }
    template <class K, class V>
    static void AddTo(TMap<K, TArray<V>>& map, const K& key, V&& value) {
        map.FindOrAdd(key).Add(MoveTempIfPossible(value));
    }
    template <class K, class V>
    static void AddTo(TMap<K, TArray<V>>& map, K&& key, V&& value) {
        map.FindOrAdd(MoveTempIfPossible(key)).Add(MoveTempIfPossible(value));
    }

    template <class K, class V>
    static TArray<V>& Find(TMap<K, TArray<V>>& map, const K& key) {
        return map.FindOrAdd(key);
    }
    template <class K, class V>
    static TArray<V>& Find(TMap<K, TArray<V>>& map, K&& key) {
        return map.FindOrAdd(MoveTempIfPossible(key));
    }
}
