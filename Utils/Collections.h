#pragma once

namespace Arrays {
    // TODO this is ugly, there must be something nicer I can do
    template <class T, class S>
    static TArray<T*> Cast(const TArray<S*> Source) {
        TArray<T*> casted;
        for (const auto s : Source)
            if (const auto u = Cast<T>(s))
                casted.Add(u);
        return MoveTemp(casted);
    }
}

namespace Actors {
    template <class T>
    static T* FindNearest(const FVector& location, TArray<T*>& actors) {  
        T* nearest = nullptr;
        double nearestDist = INFINITY;

        for (const auto actor : actors) {
            const double dist = FVector::Distance(location, actor->GetActorLocation());
            if (dist < nearestDist) {
                nearest = actor;
                nearestDist = dist;
            }
        }
        return nearest;
    }
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
