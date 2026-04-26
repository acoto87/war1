/*
    map.h - acoto87 (acoto87@gmail.com)

    MIT License

    Copyright (c) 2018 Alejandro Coto Gutiérrez

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Single-header macro library to declare and define strongly typed hash maps.
    Callers provide hash and equality hooks for the generated key type.

    USAGE
    Declare a concrete map type with shlDeclareMap(name, keyType, valueType),
    then place shlDefineMap(name, keyType, valueType) in exactly one C file.

    CUSTOMISATION
    Supply a hash function and equality function for the key type, plus a
    default value for failed lookups and an optional free function for owned
    values. Keys and values are stored by copy.

    NOTES
    This map uses open addressing with linked collision chains stored inside
    the entry array. Call Free to release internal storage. Remove and Clear
    invoke the value free hook when one is configured.

    This implementation of the macro is a variant of: https://github.com/mystborn/GenericMap
    to make a closed implementation of the map data structure, where each collision is resolved
    by keeping the index of the next element in the array of cells, and not by merely iterate
    until we find an empty cell.

    A detailed explanation of the hash function can be found here:
    https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/

    The specific constant was found here:
    http://book.huihoo.com/data-structures-and-algorithms-with-object-oriented-design-patterns-in-c++/html/page214.html
*/

#ifndef SHL_MAP_H
#define SHL_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef SHL_MALLOC
#define SHL_MALLOC(sz, userData) malloc(sz)
#endif
#ifndef SHL_CALLOC
#define SHL_CALLOC(n, sz, userData) calloc((n), (sz))
#endif
#ifndef SHL_REALLOC
#define SHL_REALLOC(ptr, sz, userData) realloc((ptr), (sz))
#endif
#ifndef SHL_FREE
#define SHL_FREE(ptr, userData) free(ptr)
#endif

#define shlDeclareMap(typeName, keyType, valueType) \
    typedef struct \
    { \
        valueType defaultValue; \
        uint32_t (*hashFn)(keyType key); \
        bool (*equalsFn)(keyType item1, keyType item2); \
        void (*freeFn)(valueType item, void* userData); \
        void* userData; \
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
        int32_t count; \
        int32_t capacity; \
        int32_t loadFactor; \
        int32_t shift; \
        uint32_t (*hashFn)(keyType key); \
        bool (*equalsFn)(keyType item1, keyType item2); \
        void (*freeFn)(valueType item, void* userData); \
        void* userData; \
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
    static int32_t typeName ## __fibHash(uint32_t hash, int32_t shift) \
    { \
        const uint32_t hashConstant = 2654435769u; \
        return (int32_t)((hash * hashConstant) >> shift); \
    } \
    \
    static void typeName ## __resize(typeName* map); \
    \
    static int32_t typeName ## __findEmptyBucket(typeName* map, int32_t index) \
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
        uint32_t hash; \
        int32_t index; \
        int32_t next; \
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
                    map->freeFn(currentValue, map->userData); \
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
                    map->freeFn(currentValue, map->userData); \
                \
                return; \
            } \
        } \
        \
        next = typeName ## __findEmptyBucket(map, index); \
        if (next < 0) \
        { \
            typeName ## __resize(map); \
            typeName ## __insert(map, key, value); \
            return; \
        } \
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
        int32_t oldCapacity = map->capacity; \
        typeName ## __Entry__* old = map->entries; \
        \
        map->loadFactor = oldCapacity; \
        map->capacity = 1 << (32 - (--map->shift)); \
        map->entries = (typeName ## __Entry__*)SHL_CALLOC(map->capacity, sizeof(typeName ## __Entry__), map->userData); \
        map->count = 0; \
        \
        for(int32_t i = 0; i < oldCapacity; i++) \
        { \
            if(old[i].active) \
                typeName ## __insert(map, old[i].key, old[i].value); \
        } \
        SHL_FREE(old, map->userData); \
    } \
    \
    void typeName ## Init(typeName* map, typeName ## Options options) \
    { \
        map->defaultValue = options.defaultValue; \
        map->hashFn = options.hashFn; \
        map->equalsFn = options.equalsFn; \
        map->freeFn = options.freeFn; \
        map->userData = options.userData; \
        map->shift = 29; \
        map->capacity = 8; \
        map->loadFactor = 6; \
        map->count = 0; \
        map->entries = (typeName ## __Entry__ *)SHL_CALLOC(map->capacity, sizeof(typeName ## __Entry__), map->userData); \
    } \
    \
    void typeName ## Free(typeName* map) \
    { \
        if (!map->entries) \
            return; \
        \
        typeName ## Clear(map); \
        \
        SHL_FREE(map->entries, map->userData); \
        map->entries = 0; \
    } \
    \
    bool typeName ## Contains(typeName* map, keyType key) \
    { \
        if (!map->entries) \
            return false; \
        \
        int32_t index; \
        uint32_t hash; \
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
        int32_t index; \
        uint32_t hash; \
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
        int32_t prevIndex, index; \
        uint32_t hash; \
        hash = prevIndex = index = typeName ## __fibHash(map->hashFn(key), map->shift); \
        \
        while (map->entries[index].active) \
        { \
            if(map->entries[index].hash == hash && map->equalsFn(map->entries[index].key, key)) \
            { \
                valueType value = map->entries[index].value; \
                int32_t nextIndex = map->entries[index].next; \
                if (nextIndex >= 0) \
                { \
                    map->entries[index] = map->entries[nextIndex]; \
                    map->entries[nextIndex].value = map->defaultValue; \
                    map->entries[nextIndex].next = -1; \
                    map->entries[nextIndex].active = false; \
                } \
                else \
                { \
                    if (prevIndex != index) \
                        map->entries[prevIndex].next = -1; \
                    map->entries[index].value = map->defaultValue; \
                    map->entries[index].next = -1; \
                    map->entries[index].active = false; \
                } \
                \
                if (map->freeFn) \
                    map->freeFn(value, map->userData); \
                \
                map->count--; \
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
    } \
    \
    void typeName ## Clear(typeName* map) \
    { \
        if (!map->entries) \
            return; \
        \
        for(int32_t i = 0; i < map->capacity; i++) \
        { \
            if (map->entries[i].active) \
            { \
                if (map->freeFn) \
                    map->freeFn(map->entries[i].value, map->userData); \
                \
                map->entries[i].value = map->defaultValue; \
                map->entries[i].next = -1; \
                map->entries[i].active = false; \
            } \
        } \
        \
        map->count = 0; \
    }

#endif //SHL_MAP_H
