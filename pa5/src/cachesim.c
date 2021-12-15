#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum CacheAssoc {direct, assoc};
enum CachePolicy {fifo, lru};

struct Cache {
    enum CachePolicy cachePolicy;
    unsigned long memoryReads;
    unsigned long memoryWrites;
    unsigned long cacheHits;
    unsigned long cacheMisses;
    unsigned long cacheLines; // Total Lines In Entire Cache
    unsigned long *cacheType; // cacheType[1] = cacheLines per set
    size_t cacheSize;
    size_t blockSize;
    size_t totalSets;

    unsigned long ***cacheData;
};

int calculateBase2Log(unsigned long value) {
    int result = 0;

    while (value >>= 1) {
        result++;
    }

    return result;
}

void buildCacheArray(struct Cache *cache, size_t setSize, size_t linesSize) {
    cache->cacheData = (unsigned long ***) malloc(sizeof(unsigned long **) * setSize);

    for (int x = 0; x < setSize; x++) {
        cache->cacheData[x] = (unsigned long **) malloc(sizeof(unsigned long *) * linesSize);

        for (int y = 0; y < linesSize; y++) {
            cache->cacheData[x][y] = (unsigned long *) malloc(sizeof(unsigned long) * 4); // The last element is going to be a clock. This can be optimized, but we then run into the dilemma of easier to code vs. speed vs. memory.
            cache->cacheData[x][y][0] = 0;
        }
    }
}

int searchCache(unsigned long ***cacheArr, unsigned long setIndex, unsigned long lineSize, unsigned long tag) {
    for (int x = 0; x < lineSize; x++) {
        if (cacheArr[setIndex][x][0] == 1 && cacheArr[setIndex][x][1] == tag) {
            return x;
        }
    }

    return -1;
}

void incrementClock(struct Cache *cache, enum CachePolicy cp) {
    for (unsigned long x = 0; x < cache->totalSets; x++) {
        for (unsigned long y = 0; y < cache->cacheType[1]; y++) {
            if (cache->cacheData[x][y][0] == 1) {
                cache->cacheData[x][y][3]++;
            }
        }
    }
}

void loadIntoCache(struct Cache *cache, unsigned long setIndex, unsigned long tag, enum CachePolicy cp) {
    incrementClock(cache, cp);
    int greatestClock = -1;
    int lineIndex = -1;

    for (int x = 0; x < cache->cacheType[1]; x++) {
        if (cache->cacheData[setIndex][x][0] == 0) {
            cache->cacheData[setIndex][x][0] = 1;
            cache->cacheData[setIndex][x][1] = tag;
            cache->cacheData[setIndex][x][2] = setIndex;
            cache->cacheData[setIndex][x][3] = 0;
            return;
        }

        if ((int) cache->cacheData[setIndex][x][3] >= greatestClock) {
            greatestClock = (int) cache->cacheData[setIndex][x][3];
            lineIndex = x;
        }


    }

    cache->cacheData[setIndex][lineIndex][0] = 1;
    cache->cacheData[setIndex][lineIndex][1] = tag;
    cache->cacheData[setIndex][lineIndex][2] = setIndex;
    cache->cacheData[setIndex][lineIndex][3] = 0;
}


void generateCache(struct Cache *cache, unsigned mode, unsigned long tag, enum CachePolicy cp, int preFetch) {
    int blockBits = calculateBase2Log(cache->blockSize);
    int setBits = calculateBase2Log(cache->totalSets);

    unsigned long setIndex;
    tag >>= blockBits;
    setIndex = ((1 << setBits) - 1) & tag;

    unsigned long nextTag = tag + 1;
    unsigned long nextSetIndex;
    nextSetIndex = ((1 << setBits) - 1) & nextTag;

    tag >>= setBits;
    nextTag >>= setBits;

    if (mode == 82) {
        int lineIndex = searchCache(cache->cacheData, setIndex, cache->cacheType[1], tag);

        if (lineIndex >= 0) { // If found...
            cache->cacheHits++;
            incrementClock(cache, cp);

            if (cp == lru) {
                cache->cacheData[setIndex][lineIndex][3] = 0;
            }

            return;
        } else if (lineIndex == -1) { // If not found...
            cache->memoryReads++;
            cache->cacheMisses++;
            loadIntoCache(cache, setIndex, tag, cp);

            if (preFetch == 1) {
                int nextLineIndex = searchCache(cache->cacheData, nextSetIndex, cache->cacheType[1], nextTag);

                if (nextLineIndex == -1) {
                    cache->memoryReads++;
                    loadIntoCache(cache, nextSetIndex, nextTag, cp);
                }
            }
        }
    } else if (mode == 87) {
        cache->memoryWrites++;

        int lineIndex = searchCache(cache->cacheData, setIndex, cache->cacheType[1], tag);

        if (lineIndex >= 0) {
            cache->cacheHits++;
            incrementClock(cache, cp);

            if (cp == lru) {
                cache->cacheData[setIndex][lineIndex][3] = 0;
            }

            return;
        } else if (lineIndex == -1) {
            cache->cacheMisses++;
            cache->memoryReads++;
            loadIntoCache(cache, setIndex, tag, cp);

            if (preFetch == 1) {
                int nextLineIndex = searchCache(cache->cacheData, nextSetIndex, cache->cacheType[1], nextTag);

                if (nextLineIndex == -1) {
                    cache->memoryReads++;
                    loadIntoCache(cache, nextSetIndex, nextTag, cp);
                }
            }
        }
    }
}

void printCaches(struct Cache *cache, struct Cache *preFetchCache) {
    printf("Prefetch 0\n");
    printf("Memory reads: %ld\n", cache->memoryReads);
    printf("Memory writes: %ld\n", cache->memoryWrites);
    printf("Cache hits: %ld\n", cache->cacheHits);
    printf("Cache misses: %ld\n", cache->cacheMisses);
    printf("Prefetch 1\n");
    printf("Memory reads: %ld\n", preFetchCache->memoryReads);
    printf("Memory writes: %ld\n", preFetchCache->memoryWrites);
    printf("Cache hits: %ld\n", preFetchCache->cacheHits);
    printf("Cache misses: %ld\n", preFetchCache->cacheMisses);
}

void startCacheEmulation(struct Cache *cache, char **arguments, enum CachePolicy cp, int preFetch) {
    cache->cacheSize = atoi(arguments[1]);
    cache->blockSize = atoi(arguments[4]);
    cache->cacheLines = cache->cacheSize / cache->blockSize;

    if (strcmp(arguments[2], "direct") == 0) {
        cache->cacheType = (unsigned long *) malloc(sizeof(unsigned long) * 2);
        cache->cacheType[0] = (unsigned long) direct; // One line, many sets.
        cache->cacheType[1] = 1; // One line per set.
        cache->totalSets = cache->cacheLines;
    } else if (strcmp(arguments[2], "assoc") == 0) {
        cache->cacheType = (unsigned long *) malloc(sizeof(unsigned long) * 2);
        cache->cacheType[0] = (unsigned long) assoc;
        cache->cacheType[1] = cache->cacheLines; // One set, crap ton of lines.
        cache->totalSets = 1;
    } else if (strcmp(arguments[2], "assoc:") > 0) {
        cache->cacheType = (unsigned long *) malloc(sizeof(unsigned long) * 2);
        cache->cacheType[0] = (unsigned long) assoc;
        cache->cacheType[1] = atoi(&arguments[2][6]); // Multiple sets, multiple lines.
        cache->totalSets = cache->cacheLines / cache->cacheType[1];
    } else {
        perror("Invalid argument entered in argv[2].");
        exit(1);
    }

    cache->memoryReads = cache->memoryWrites = cache->cacheHits = cache->cacheMisses = 0;
    buildCacheArray(cache, cache->totalSets, cache->cacheType[1]);
}

void freeEveryThing(struct Cache *cache, struct Cache *preFetchCache) {
    for (int x = 0; x < cache->totalSets; x++) {
        for (int y = 0; y < cache->cacheType[1]; y++) {
            free(cache->cacheData[x][y]);
            free(preFetchCache->cacheData[x][y]);
        }

        free(cache->cacheData[x]);
        free(preFetchCache->cacheData[x]);
    }

    free(cache->cacheData);
    free(preFetchCache->cacheData);

    free(cache->cacheType);
    free(preFetchCache->cacheType);

}

void processFile(FILE *file, struct Cache *cache, struct Cache *preFetchCache, enum CachePolicy cp, char **arguments) {
    char throwAway[33]; // Considering 48 bits, no address should be greater than 100.
    unsigned long index = 0;

    startCacheEmulation(cache, arguments, cp, 0);
    startCacheEmulation(preFetchCache, arguments, cp, 0);

    while (fscanf(file, "%32s", throwAway) != EOF) {
        if (strcmp(throwAway, "#eof") == 0) {
            break;
        }

        char mode;
        unsigned long address;

        if (fscanf(file, " %c", &mode) != EOF && fscanf(file, "%lx", &address) != EOF) {
            generateCache(cache, mode, address, cp, 0);
            generateCache(preFetchCache, mode, address, cp, 1);
        }

        index++;
    }
}

int main(int argc, char **argv) {
    enum CachePolicy cp; // Could probably convert this into a checkArguments() function, but for the purposes of this assignment here, it is not necessary.

    if (strcmp(argv[3], "fifo") == 0) {
        cp = fifo;
    } else if (strcmp(argv[3], "lru") == 0) {
        cp = lru;
    } else {
        perror("Invalid cache policy entered. Exiting programming...");
        exit(1);
    }

    struct Cache cache;
    struct Cache preFetchCache;

    FILE *file = fopen(argv[argc - 1], "r");
    processFile(file, &cache, &preFetchCache, cp, argv); // = fileData

    printCaches(&cache, &preFetchCache);
    freeEveryThing(&cache, &preFetchCache);
}