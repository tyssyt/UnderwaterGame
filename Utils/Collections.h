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
