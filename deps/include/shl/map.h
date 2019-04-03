/*
 * This implementation of the macro is a variant of: https://github.com/mystborn/GenericMap
 * to make a closed implementation of the map data structure, where each collision is resolved
 * by keeping the index of the next element in the array of cells, and not by merely iterate
 * until we find an empty cell.
 * 
 * A detailed explanation of the hash function can be found here: 
 * https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/
 *
 * The specific constant was found here:
 * http://book.huihoo.com/data-structures-and-algorithms-with-object-oriented-design-patterns-in-c++/html/page214.html
*/

#ifndef SHL_MAP_H
#define SHL_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define shlDeclareMap(typeName, keyType, valueType) \
    typedef struct \
    { \
        valueType defaultValue; \
        uint32_t (*hashFn)(const keyType key); \
        bool (*equalsFn)(const keyType item1, const keyType item2); \
        void (*freeFn)(valueType item); \
    } typeName ## Options; \
    \
    typedef struct { \
        bool active; \
        uint32_t hash; \
        int32_t next; \
        keyType key; \
        valueType value; \
    } typeName ## __Entry__; \
    \
    typedef struct { \
        uint32_t count; \
        uint32_t capacity; \
        uint32_t loadFactor; \
        uint32_t shift; \
        uint32_t (*hashFn)(const keyType key); \
        bool (*equalsFn)(const keyType item1, const keyType item2); \
        void (*freeFn)(valueType item); \
        valueType defaultValue; \
        typeName ## __Entry__* entries; \
    } typeName; \
    \
    void typeName ## Init(typeName* map, typeName ## Options options); \
    void typeName ## Free(typeName* map); \
    bool typeName ## Contains(typeName* map, keyType key); \
    valueType typeName ## Get(typeName* map, keyType key); \
    void typeName ## Set(typeName* map, keyType key, valueType value); \
    void typeName ## Remove(typeName* map, keyType key); \
    void typeName ## Clear(typeName* map);

#define shlDefineMap(typeName, keyType, valueType) \
    static uint32_t typeName ## __fibHash(uint32_t hash, uint32_t shift) \
    { \
        const uint32_t hashConstant = 2654435769u; \
        return (hash * hashConstant) >> shift; \
    } \
    \
    static uint32_t typeName ## __findEmptyBucket(typeName* map, uint32_t index) \
    { \
        for(int32_t i = 0; i < map->capacity; i++) \
        { \
            if (!map->entries[(index + i) % map->capacity].active) \
                return (index + i) % map->capacity; \
        } \
        \
        return -1; \
    } \
    \
    static void typeName ## __insert(typeName* map, keyType key, valueType value) \
    { \
        uint32_t hash, index, next; \
        hash = index = typeName ## __fibHash(map->hashFn(key), map->shift); \
        \
        while (map->entries[index].active && map->entries[index].next >= 0) \
        { \
            if(map->entries[index].hash == hash && map->equalsFn(map->entries[index].key, key)) \
            { \
                valueType currentValue = map->entries[index].value; \
                map->entries[index].value = value; \
                \
                if (map->freeFn) \
                    map->freeFn(currentValue); \
                \
                return; \
            } \
            \
            index = map->entries[index].next; \
        } \
        \
        if (map->entries[index].active) \
        { \
            if(map->entries[index].hash == hash && map->equalsFn(map->entries[index].key, key)) \
            { \
                valueType currentValue = map->entries[index].value; \
                map->entries[index].value = value; \
                \
                if (map->freeFn) \
                    map->freeFn(currentValue); \
                \
                return; \
            } \
        } \
        \
        next = typeName ## __findEmptyBucket(map, index); \
        if (index != next) \
            map->entries[index].next = next; \
        \
        map->entries[next].active = true; \
        map->entries[next].key = key; \
        map->entries[next].value = value; \
        map->entries[next].hash = hash; \
        map->entries[next].next = -1; \
        map->count++; \
    } \
    \
    static void typeName ## __resize(typeName* map) \
    { \
        uint32_t oldCapacity = map->capacity; \
        typeName ## __Entry__* old = map->entries; \
        \
        map->loadFactor = oldCapacity; \
        map->capacity = 1 << (32 - (--map->shift)); \
        map->entries = calloc(map->capacity, sizeof(typeName ## __Entry__)); \
        map->count = 0; \
        \
        for(int32_t i = 0; i < oldCapacity; i++) \
        { \
            if(old[i].active) \
                typeName ## __insert(map, old[i].key, old[i].value); \
        } \
        free(old); \
    } \
    \
    void typeName ## Init(typeName* map, typeName ## Options options) \
    { \
        map->defaultValue = options.defaultValue; \
        map->hashFn = options.hashFn; \
        map->equalsFn = options.equalsFn; \
        map->freeFn = options.freeFn; \
        map->shift = 29; \
        map->capacity = 8; \
        map->loadFactor = 6; \
        map->count = 0; \
        map->entries = (typeName ## __Entry__ *)calloc(map->capacity, sizeof(typeName ## __Entry__)); \
    } \
    \
    void typeName ## Free(typeName* map) \
    { \
        if (!map->entries) \
            return; \
        \
        typeName ## Clear(map); \
        \
        free(map->entries); \
        map->entries = 0; \
    } \
    \
    bool typeName ## Contains(typeName* map, keyType key) \
    { \
        if (!map->entries) \
            return false; \
        \
        uint32_t index, hash; \
        hash = index = typeName ## __fibHash(map->hashFn(key), map->shift); \
        \
        bool found = false; \
        \
        while (map->entries[index].active) \
        { \
            if(map->entries[index].hash == hash && map->equalsFn(map->entries[index].key, key)) \
            { \
                found = true; \
                break; \
            } \
            \
            if (map->entries[index].next < 0) \
            { \
                break; \
            } \
            \
            index = map->entries[index].next; \
        } \
        \
        return found; \
    } \
    \
    valueType typeName ## Get(typeName* map, keyType key) \
    { \
        if (!map->entries) \
            return map->defaultValue; \
        \
        uint32_t index, hash; \
        hash = index = typeName ## __fibHash(map->hashFn(key), map->shift); \
        \
        valueType value = map->defaultValue; \
        \
        while (map->entries[index].active) \
        { \
            if(map->entries[index].hash == hash && map->equalsFn(map->entries[index].key, key)) \
            { \
                value = map->entries[index].value; \
                break; \
            } \
            \
            if (map->entries[index].next < 0) \
            { \
                break; \
            } \
            \
            index = map->entries[index].next; \
        } \
        \
        return value; \
    } \
    \
    void typeName ## Set(typeName* map, keyType key, valueType value) \
    { \
        if (!map->entries) \
            return; \
        \
        if(map->count == map->loadFactor) \
            typeName ## __resize(map); \
        \
        typeName ## __insert(map, key, value); \
    } \
    \
    void typeName ## Remove(typeName* map, keyType key) \
    { \
        if (!map->entries) \
            return; \
        \
        uint32_t prevIndex, index, hash; \
        \
        hash = prevIndex = index = typeName ## __fibHash(map->hashFn(key), map->shift); \
        \
        while (map->entries[index].active) \
        { \
            if(map->entries[index].hash == hash && map->equalsFn(map->entries[index].key, key)) \
            { \
                valueType value = map->entries[index].value; \
                \
                map->entries[prevIndex].next = map->entries[index].next; \
                map->entries[index].value = map->defaultValue; \
                map->entries[index].active = false; \
                \
                if (map->freeFn) \
                    map->freeFn(value); \
                \
                break; \
            } \
            \
            if (map->entries[index].next < 0) \
            { \
                break; \
            } \
            \
            prevIndex = index; \
            index = map->entries[index].next; \
        } \
        map->count--; \
    } \
    \
    void typeName ## Clear(typeName* map) \
    { \
        if (!map->entries) \
            return; \
        \
        if (map->freeFn) \
        { \
            for(int32_t i = 0; i < map->count; i++) \
            { \
                if (map->entries[i].active) \
                { \
                    map->freeFn(map->entries[i].value); \
                    map->entries[i].active = false; \
                } \
            } \
        } \
        \
        map->count = 0; \
    }

#endif //SHL_MAP_H