#ifndef JRAND_H
#define JRAND_H

#include <stdint.h>

static inline uint64_t jr_rotl(const uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint32_t jr_rotl32(const uint32_t x, int k) { return (x << k) | (x >> (32 - k)); }
static inline uint64_t jr_rotl64(const uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }

// xorshift

static inline uint64_t jr_xorshift64(uint64_t *state) {
    uint64_t x = *state;
    if (x == 0) { x = 1; }

    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;

    *state = x;
    return x;
}

static inline uint32_t jr_xorshift32(uint32_t *state) {
    uint32_t x = *state;
    if (x == 0) { x = 1; }

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    *state = x;
    return x;
}

// xoshiro

static inline uint32_t jr_xoshiro128ss(uint32_t *state) {
    uint32_t res = jr_rotl32(state[1] * 5, 7) * 9;
    uint32_t t = state[1] << 9;

    state[2] ^= state[0];
    state[3] ^= state[1];
    state[1] ^= state[2];
    state[0] ^= state[3];

    state[2] ^= t;
    state[3] = jr_rotl32(state[3], 11);

    return res;
}

static inline uint64_t jr_xoshiro256ss(uint64_t *state) {
    uint64_t res = jr_rotl64(state[1] * 5, 7) * 9;
    uint64_t t = state[1] << 17;

    state[2] ^= state[0];
    state[3] ^= state[1];
    state[1] ^= state[2];
    state[0] ^= state[3];

    state[2] ^= t;
    state[3] = jr_rotl64(state[3], 45);

    return res;
}

// splitmix

#define JR_SPLITMIX64_GAMMA 0x9E3779B97F4A7C15ULL
#define JR_SPLITMIX32_GAMMA 0x9E3779B9U

#define JR_SPLITMIX64_MIX1 0xBF58476D1CE4E5B9ULL
#define JR_SPLITMIX64_MIX2 0x94D049BB133111EBULL
#define JR_SPLITMIX32_MIX1 0x21F0AAADU
#define JR_SPLITMIX32_MIX2 0x735A2D97U

static inline uint64_t jr_splitmix64(uint64_t *state) {
    uint64_t z = (*state += JR_SPLITMIX64_GAMMA);

    z = (z ^ (z >> 30)) * JR_SPLITMIX64_MIX1;
    z = (z ^ (z >> 27)) * JR_SPLITMIX64_MIX2;

    return z ^ (z >> 31);
}

static inline uint32_t jr_splitmix32(uint32_t *state) {
    uint32_t z = (*state += JR_SPLITMIX32_GAMMA);

    z = (z ^ (z >> 16)) * JR_SPLITMIX32_MIX1;
    z = (z ^ (z >> 15)) * JR_SPLITMIX32_MIX2;

    return z ^ (z >> 15);
}

// abs

typedef uint32_t (*jr_rand32_fn)(uint32_t *state);
typedef uint64_t (*jr_rand64_fn)(uint64_t *state);

static inline uint32_t jr_range32(jr_rand32_fn randfc, uint32_t *state, uint32_t min, uint32_t max) {
    uint32_t range = max - min + 1U;

    if (range == 0) { return randfc(state); }

    uint32_t threshold = -range % range;

    uint32_t r;
    do {
        r = randfc(state);
    } while (r < threshold);

    return min + (r % range);
}

static inline uint64_t jr_range64(jr_rand64_fn randfc, uint64_t *state, uint64_t min, uint64_t max) {
    uint64_t range = max - min + 1ULL;

    if (range == 0) { return randfc(state); }

    uint64_t threshold = -range % range;

    uint64_t r;
    do {
        r = randfc(state);
    } while (r < threshold);

    return min + (r % range);
}

#endif // JRAND_H
