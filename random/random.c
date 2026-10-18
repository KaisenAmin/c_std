/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Random
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



/* Internal PRNG state. The module now owns its generator (xorshift32)
   instead of relying on libc rand(), so getstate/setstate can actually
   snapshot and restore the sequence — rand()'s state is opaque and not
   round-trippable through a single integer.

   The state is advanced with an atomic compare-and-swap, so concurrent
   callers never lose an update or receive the same draw (a plain
   read-modify-write raced). Draws from several threads still interleave in
   an unspecified order; seed per thread-independent run for reproducibility. */
static unsigned int rand_state = 1u;

#if defined(_MSC_VER) && !defined(__clang__)
    #include <intrin.h>
    static unsigned int rs_load(void) {
        return (unsigned int)_InterlockedCompareExchange((volatile long*)&rand_state, 0, 0);
    }
    static void rs_store(unsigned int v) {
        _InterlockedExchange((volatile long*)&rand_state, (long)v);
    }
    static int rs_cas(unsigned int* expected, unsigned int desired) {
        long prev = _InterlockedCompareExchange((volatile long*)&rand_state, (long)desired, (long)*expected);
        if ((unsigned int)prev == *expected) {
            return 1;
        }
        *expected = (unsigned int)prev;
        return 0;
    }
#else
    static unsigned int rs_load(void) {
        return __atomic_load_n(&rand_state, __ATOMIC_RELAXED);
    }
    static void rs_store(unsigned int v) {
        __atomic_store_n(&rand_state, v, __ATOMIC_RELAXED);
    }
    static int rs_cas(unsigned int* expected, unsigned int desired) {
        return __atomic_compare_exchange_n(&rand_state, expected, desired, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
    }
#endif

/* One Marsaglia xorshift32 step. */
static unsigned int prng_step(unsigned int x) {
    if (x == 0) {
        x = 2463534242u;  /* xorshift requires non-zero state */
    }

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

/* Advance and return the next 32-bit value. */
static unsigned int prng_next32(void) {
    unsigned int old = rs_load();
    unsigned int x;
    do {
        x = prng_step(old);
    } while (!rs_cas(&old, x));

    return x;
}

static unsigned long long prng_next64(void) {
    unsigned long long hi = (unsigned long long)prng_next32() << 32;
    return hi | (unsigned long long)prng_next32();
}

/* Uniform integer in [0, n), n >= 1. The lowest (2^32 mod n) raw values are
   rejected so every result is equally likely: a plain `% n` favoured the low
   residues (up to 2:1 for large n). At most half of the draws are rejected. */
static unsigned int prng_below32(unsigned int n) {
    unsigned int threshold = (0u - n) % n;
    for (;;) {
        unsigned int r = prng_next32();
        if (r >= threshold) {
            return r % n;
        }
    }
}

static unsigned long long prng_below64(unsigned long long n) {
    unsigned long long threshold = (0ull - n) % n;
    for (;;) {
        unsigned long long r = prng_next64();
        if (r >= threshold) {
            return r % n;
        }
    }
}


/* Internal helper: unbiased random size_t in [0, n). Used by shuffle/choice/sample. */
static size_t random_index(size_t n) {
    if (n == 0) {
        return 0;
    }
    if (n <= 0xFFFFFFFFu) {
        return (size_t)prng_below32((unsigned int)n);
    }

    return (size_t)prng_below64((unsigned long long)n);
}


/* Turns a seed into a generator state. xorshift32 is linear, so using small
   seeds raw gave strongly correlated streams (the first random() was below
   0.1 for every seed 1..1000), and seed 0 was the same stream as seed 1. A
   bijective 32-bit mixer (lowbias32) of seed + golden ratio fixes both. */
static unsigned int random_seed_to_state(unsigned int seed) {
    unsigned int x = seed + 0x9E3779B9u;
    x ^= x >> 16;
    x *= 0x7FEB352Du;
    x ^= x >> 15;
    x *= 0x846CA68Bu;
    x ^= x >> 16;
    return x ? x : 0x6D2B79F5u;
}


/**
 * @brief This function sets the seed for the random number generator, allowing
 *      for reproducible sequences of random numbers.
 *
 * @param seed The seed value to initialize the random number generator.
 */
void random_seed(unsigned int seed) {
    RANDOM_LOG("[random_seed]: Entering with seed: %u", seed);

    /* Seed both the internal xorshift state and libc rand(). Internal
       generator is what now drives every public random_* function;
       libc rand() is kept seeded for any caller that still pokes at it. */
    rs_store(random_seed_to_state(seed));
    srand(seed);

    RANDOM_LOG("[random_seed]: Seed set to: %u", seed);
    RANDOM_LOG("[random_seed]: Exiting");
}


/**
 * @brief This function generates a random integer in the range [a, b], inclusive.
 * if `a` is greater than `b`, their values are swapped to ensure the correct range.
 *
 * @param a The lower bound of the range (inclusive).
 * @param b The upper bound of the range (inclusive).
 * 
 * @return A random integer within the range [a, b].
 */
int random_randint(int a, int b) {
    RANDOM_LOG("[random_randint]: Entering with a: %d, b: %d", a, b);

    if (a > b) {
        RANDOM_LOG("[random_randint]: Swapping a and b as a > b.");
        int temp = a;
        a = b;
        b = temp;
    }

    /* The span is computed in 64 bits: `b - a + 1` in int overflowed for
       spans above INT_MAX and was 0 (division by zero) for the full range. */
    long long span = (long long)b - (long long)a + 1;
    unsigned long long offset = (span > 0xFFFFFFFFLL) ? (unsigned long long)prng_next32() : (unsigned long long)prng_below32((unsigned int)span);
    int result = (int)((long long)a + (long long)offset);
    RANDOM_LOG("[random_randint]: Generated random integer: %d in range [%d, %d]", result, a, b);
    RANDOM_LOG("[random_randint]: Exiting with result: %d", result);

    return result;
}


/**
 * @brief This function generates a random integer in the range `[a, b)` with a specified step.
 *
 * @param a The start of the range (inclusive).
 * @param b The end of the range (exclusive).
 * @param step The step value (can be positive or negative, but not zero).
 *
 * @return A random integer within the range `[a, b)` with the given step,
 *         or -1 for an empty range (`a == b`, `step == 0`, or a step pointing
 *         away from `b`). -1 can also be a valid result (randrange(-5, 0, 1)),
 *         so use random_randrange_checked when that matters.
 */
int random_randrange(int a, int b, int step) {
    int out;
    return random_randrange_checked(a, b, step, &out) ? out : -1;
}


/**
 * @brief random_randrange with an unambiguous error report.
 *
 * @param out Receives the random value on success (left unchanged otherwise).
 * @return true on success; false for an empty range (a == b, step == 0, or a
 *         step that points away from b) or a NULL @p out.
 */
bool random_randrange_checked(int a, int b, int step, int* out) {
    RANDOM_LOG("[random_randrange]: Entering with a: %d, b: %d, step: %d", a, b, step);

    if (out == NULL || a == b || step == 0) {
        RANDOM_LOG("[random_randrange]: Error: invalid range (a == b) or step (step == 0).");
        return false;
    }
    if ((step > 0 && a > b) || (step < 0 && a < b)) {
        RANDOM_LOG("[random_randrange]: Error: logic error with range and step values.");
        return false;
    }

    /* 64-bit arithmetic: abs(b - a) and a + k * step overflowed int for wide
       ranges (randrange(INT_MIN, INT_MAX, 1) always returned INT_MIN). The
       step count is at most 2^32 - 1. */
    long long range = llabs((long long)b - (long long)a);
    long long astep = llabs((long long)step);
    long long num_steps = (range + astep - 1) / astep;
    RANDOM_LOG("[random_randrange]: Calculated num_steps: %lld", num_steps);

    long long random_step = (long long)prng_below32((unsigned int)num_steps);
    RANDOM_LOG("[random_randrange]: Generated random_step: %lld", random_step);

    int result = (int)((long long)a + random_step * (long long)step);
    RANDOM_LOG("[random_randrange]: Exiting with result: %d", result);

    *out = result;
    return true;
}


/**
 * @brief This function generates a random double precision floating-point number
 *      in the range [0, 1) using the standard library `rand()` function.
 *
 * @return A random double in the range [0, 1).
 */
double random_random() {
    RANDOM_LOG("[random_random]: Entering");

    /* 2^32 in the denominator so the value is strictly less than 1.0. */
    double result = (double)prng_next32() / 4294967296.0;
    RANDOM_LOG("[random_random]: Generated random double: %f in range [0, 1)", result);

    RANDOM_LOG("[random_random]: Exiting with result: %f", result);
    return result;
}


/**
 * @brief Performs a Bernoulli trial: returns `true` with probability @p p and
 *        `false` with probability `1 - p`.
 *
 * This is the reproducible "weighted coin flip" — the building block for
 * probabilistic decisions such as a percentage feature rollout, Monte-Carlo
 * acceptance steps, or randomized sampling. Like every other `random_*`
 * function it is driven by the seeded engine (one draw per call), so a fixed
 * seed reproduces the exact sequence of outcomes.
 *
 * Boundary values are handled exactly: `p <= 0.0` always returns `false` and
 * `p >= 1.0` always returns `true` (no draw is wasted on a certain outcome).
 * The probability resolution is that of `random_random` (~2^-32).
 *
 * @param p Probability of returning `true`, expected in [0, 1]. Values
 *          outside that range are clamped to the nearest certain outcome.
 * @return `true` with probability `p`, otherwise `false`.
 *
 * @code
 * random_seed(2026);
 * if (random_boolean(0.10)) {       // ~10% of users
 *     enable_new_feature();
 * }
 * @endcode
 */
bool random_boolean(double p) {
    RANDOM_LOG("[random_boolean]: Entering with p: %f", p);

    if (p <= 0.0) {
        return false;
    }
    if (p >= 1.0) {
        return true;
    }

    bool result = random_random() < p;
    RANDOM_LOG("[random_boolean]: Exiting with result: %d", (int)result);
    return result;
}


/**
 * @brief This function generates a random double precision floating-point number
 *      in the range [a, b). If `a` is greater than `b`, their values are swapped.
 *
 * @param a The lower bound of the range (inclusive).
 * @param b The upper bound of the range (exclusive).
 * 
 * @return A random double in the range [a, b).
 */
double random_uniform(double a, double b) {
    RANDOM_LOG("[random_uniform]: Entering random_uniform with a: %f, b: %f", a, b);

    if (a > b) {
        RANDOM_LOG("[random_uniform]: Swapping a and b as a > b.");
        double temp = a;
        a = b;
        b = temp;
    }

    double result = a + ((double)prng_next32() / 4294967296.0) * (b - a);
    RANDOM_LOG("[random_uniform]: Exiting random_uniform with result: %f", result);

    return result;
}


/**
 * @brief Generates a non-negative random integer with the requested number
 *        of random bits.
 *
 * @param a Number of random bits to generate (must be 1..31 inclusive).
 *          Note: 32 is rejected because filling all 32 bits of a signed `int`
 *          would set the sign bit and produce negative results — callers
 *          expect a non-negative integer in `[0, 2^a)`.
 *
 * @return A non-negative integer in `[0, 2^a)`, or -1 on invalid `a`.
 */
int random_getrandbits(int a) {
    RANDOM_LOG("[random_getrandbits]: Entering random_getrandbits with a: %d", a);

    if (a <= 0 || a > 31) {
        RANDOM_LOG("[random_getrandbits]: Error: a is out of valid range (1 to 31).");
        return -1;
    }

    unsigned int v = prng_next32();
    int result = (int)(v & ((1U << a) - 1U));

    RANDOM_LOG("[random_getrandbits]: Exiting random_getrandbits with result: %d", result);
    return result;
}


/**
 * @brief Fills @p buffer with @p n pseudo-random bytes from the seeded engine.
 *
 * The fast, reproducible counterpart to `secrets_token_bytes` (which is
 * cryptographically secure but not reproducible): use this to generate
 * deterministic random payloads for tests, fuzzing corpora, synthetic data,
 * or any non-security buffer fill. Bytes are produced from the same xorshift32
 * stream as every other generator, four per draw, so a fixed `random_seed`
 * reproduces the exact byte sequence.
 *
 * NOT suitable for keys, tokens, nonces, or anything security-sensitive — use
 * the `secrets` module for those.
 *
 * @param buffer Destination buffer to fill. If NULL, the call is a safe no-op.
 * @param n      Number of bytes to write. If 0, the call is a safe no-op.
 *
 * @code
 * random_seed(7);
 * unsigned char payload[16];
 * random_randbytes(payload, sizeof(payload));   // 16 reproducible bytes
 * @endcode
 */
void random_randbytes(unsigned char* buffer, size_t n) {
    RANDOM_LOG("[random_randbytes]: Entering with buffer: %p, n: %zu", (void*)buffer, n);

    if (buffer == NULL || n == 0) {
        RANDOM_LOG("[random_randbytes]: NULL buffer or n == 0 -> no-op.");
        return;
    }

    size_t i = 0;
    /* Whole 32-bit words: 4 bytes per draw (little-endian extraction). */
    while (i + 4 <= n) {
        unsigned int v = prng_next32();
        buffer[i++] = (unsigned char)(v & 0xFFu);
        buffer[i++] = (unsigned char)((v >> 8) & 0xFFu);
        buffer[i++] = (unsigned char)((v >> 16) & 0xFFu);
        buffer[i++] = (unsigned char)((v >> 24) & 0xFFu);
    }
    /* Tail (1..3 leftover bytes) from one more draw. */
    if (i < n) {
        unsigned int v = prng_next32();
        while (i < n) {
            buffer[i++] = (unsigned char)(v & 0xFFu);
            v >>= 8;
        }
    }

    RANDOM_LOG("[random_randbytes]: Exiting after writing %zu bytes.", n);
}


/**
 * @brief Shuffles an array of any type.

 * @param array Pointer to the array to shuffle. if array is NULL function print error message and return nothing.
 * @param n The number of elements in the array. if n == 0 function print error message and return nothing.
 * @param size The size of each element in the array.
 * 
 * @note This function shuffles an array of any type using the Fisher-Yates algorithm.
 */
void random_shuffle(void *array, size_t n, size_t size) {
    RANDOM_LOG("[random_shuffle]: Entering random_shuffle with array: %p, n: %zu, size: %zu", (void*)array, n, size);

    if (array == NULL) {
        RANDOM_LOG("[random_shuffle]: Error: array is NULL in random_shuffle.");
        return;
    }
    if (n == 0) {
        RANDOM_LOG("[random_shuffle]: Error: size of array is zero in random_shuffle.");
        return;
    }

    char* arr = (char*)array;
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + random_index(n - i);
        RANDOM_LOG("[random_shuffle]: Swapping element %zu with element %zu", i, j);

        if (j != i) {
            /* swap through a small stack buffer: no allocation, so the
               shuffle can never stop half-way */
            unsigned char tmp[64];
            unsigned char* p = (unsigned char*)arr + i * size;
            unsigned char* q = (unsigned char*)arr + j * size;
            for (size_t done = 0; done < size; ) {
                size_t chunk = (size - done < sizeof(tmp)) ? size - done : sizeof(tmp);
                memcpy(tmp, p + done, chunk);
                memcpy(p + done, q + done, chunk);
                memcpy(q + done, tmp, chunk);
                done += chunk;
            }
        }

        RANDOM_LOG("[random_shuffle]: Completed swap between elements %zu and %zu", i, j);
    }

    RANDOM_LOG("[random_shuffle]: Exiting random_shuffle");
}


/**
 * @brief Randomly selects an element from an array and returns a pointer to an element from the array.
 *
 * @param array Pointer to the array to select from. if array is NULL function return NULL.
 * @param n The number of elements in the array. if n == 0 function return NULL.
 * @param size The size of each element in the array.
 * 
 * @return Pointer to the randomly selected element. Returns NULL for invalid input.
 */
void* random_choice(void* array, size_t n, size_t size) {
    RANDOM_LOG("[random_choice]: Entering random_choice with array: %p, n: %zu, size: %zu", (void*)array, n, size);

    if (array == NULL) {
        RANDOM_LOG("[random_choice]: Error: array is NULL in random_choice.");
        return NULL;
    }
    if (n == 0) {
        RANDOM_LOG("[random_choice]: Error: size of array is zero in random_choice.");
        return NULL;
    }

    size_t index = random_index(n);
    RANDOM_LOG("[random_choice]: Selected index: %zu", index);

    void* result = (char*)array + index * size;
    RANDOM_LOG("[random_choice]: Exiting random_choice with result: %p", (void*)result);

    return result;
}


/**
 * @brief Generates a random double number based on the Triangular distribution.
 *
 * @param low The lower limit of the distribution.
 * @param high The upper limit of the distribution.
 * @param mode The mode (peak) of the distribution. if `mode` is not within the range [low, high],
 *             it is set to the midpoint of `low` and `high`.
 * @return A random double number from the Triangular distribution.
 *
 * @note If `low` is greater than `high`, their values are swapped.
 * @note The function uses the inverse transform sampling method to produce the triangular-distributed number.
 */
double random_triangular(double low, double high, double mode) {
    RANDOM_LOG("[random_triangular]: Entering random_triangular with low: %f, high: %f, mode: %f", low, high, mode);

    if (low > high) {
        RANDOM_LOG("[random_triangular]: Swapping low and high, as low > high.");
        double temp = low;
        low = high;
        high = temp;
    }

    if (mode < low || mode > high) {
        RANDOM_LOG("[random_triangular]: Mode is out of bounds, setting mode to midpoint of low and high.");
        mode = (low + high) / 2;
    }

    double u = random_random();
    double c = (mode - low) / (high - low);
    RANDOM_LOG("[random_triangular]: Generated random number u: %f, calculated c: %f", u, c);

    double result;
    if (u <= c) {
        result = low + sqrt(u * (high - low) * (mode - low));
    }
    else {
        result = high - sqrt((1 - u) * (high - low) * (high - mode));
    }

    RANDOM_LOG("[random_triangular]: Exiting random_triangular with result: %f", result);
    return result;
}


/**
 * @brief Selects multiple random elements from an array with specified weights.
 * The selected elements are copied into the provided choices array.
 *
 * @param array Pointer to the input array from which to select elements.
 * @param n The number of elements in the input array.
 * @param size The size of each element in the input array.
 * @param num_choices The number of elements to select.
 * @param choices Pointer to the array where the selected elements will be stored.
 * @param weights Pointer to the array of weights corresponding to each element in the input array.
 *
 * @return true on success. false (and @p choices untouched) for NULL
 *         array / choices, n == 0, size == 0, num_choices == 0, or weights
 *         that are negative, not finite, or all zero (element 0 used to be
 *         picked every time then).
 *
 * @note An element with weight 0 is never chosen. Each draw is a binary
 *       search over the cumulative weights.
 */
bool random_choices(void *array, size_t n, size_t size, size_t num_choices, void *choices, double *weights) {
    RANDOM_LOG("[random_choices]: Entering random_choices with array: %p, n: %zu, size: %zu, num_choices: %zu, choices: %p, weights: %p", (void*)array, n, size, num_choices, (void*)choices, (void*)weights);

    if (array == NULL || choices == NULL || n == 0 || size == 0 || num_choices == 0) {
        RANDOM_LOG("[random_choices]: Error: invalid arguments in random_choices.");
        return false;
    }

    double *cumulative_weights = (double *)malloc(n * sizeof(double));
    if (cumulative_weights == NULL) {
        RANDOM_LOG("[random_choices]: Error: memory allocation failed for cumulative_weights in random_choices.");
        return false;
    }

    // Cumulative weights; every weight must be finite and >= 0, the total > 0
    double total = 0.0;
    for (size_t i = 0; i < n; i++) {
        double w = weights ? weights[i] : 1.0;
        if (!(w >= 0.0) || !isfinite(w)) {   /* also rejects NaN */
            RANDOM_LOG("[random_choices]: Error: weight %zu is negative or not finite.", i);
            free(cumulative_weights);
            return false;
        }
        total += w;
        cumulative_weights[i] = total;
    }
    if (!(total > 0.0) || !isfinite(total)) {
        RANDOM_LOG("[random_choices]: Error: the weights add up to zero (or overflow).");
        free(cumulative_weights);
        return false;
    }

    // Make random choices: r in [0, total); the first i with cumulative[i] > r
    for (size_t i = 0; i < num_choices; i++) {
        double r = random_random() * total;
        size_t lo = 0, hi = n - 1;
        while (lo < hi) {
            size_t mid = lo + (hi - lo) / 2;
            if (cumulative_weights[mid] > r) {
                hi = mid;
            }
            else {
                lo = mid + 1;
            }
        }
        RANDOM_LOG("[random_choices]: Selected index %zu for choice %zu", lo, i);
        memcpy((char *)choices + i * size, (char *)array + lo * size, size);
    }

    free(cumulative_weights);
    return true;
}


/**
 * @brief Samples a specified number of unique random elements from an array.
 * The sampled elements are copied into the provided samples array.
 *
 * @param array Pointer to the input array from which to sample elements.
 * @param n The number of elements in the input array.
 * @param size The size of each element in the input array.
 * @param num_samples The number of elements to sample.
 * @param samples Pointer to the array where the sampled elements will be stored.
 *
 * @note The function performs error checking to ensure valid inputs. If any error is detected,
 *       an error message is printed and the function returns without performing sampling.
 * 
 * @note This function uses a Fisher-Yates shuffle algorithm to ensure unique random sampling.
 */
void random_sample(void *array, size_t n, size_t size, size_t num_samples, void *samples) {
    RANDOM_LOG("[random_sample]: Entering random_sample with array: %p, n: %zu, size: %zu, num_samples: %zu, samples: %p", (void*)array, n, size, num_samples, (void*)samples);

    if (array == NULL) {
        RANDOM_LOG("[random_sample]: Error: array is NULL in random_sample.");
        return;
    }
    if (n == 0) {
        RANDOM_LOG("[random_sample]: Error: size of array is zero in random_sample.");
        return;
    }
    if (num_samples == 0) {
        RANDOM_LOG("[random_sample]: Error: number of samples is zero in random_sample.");
        return;
    }
    if (num_samples > n) {
        RANDOM_LOG("[random_sample]: Error: number of samples is greater than the number of elements in array.");
        return;
    }

    size_t *indices = (size_t *)malloc(n * sizeof(size_t));
    if (indices == NULL) {
        RANDOM_LOG("[random_sample]: Error: memory allocation failed for indices in random_sample.");
        return;
    }

    RANDOM_LOG("[random_sample]: Initializing indices.");
    for (size_t i = 0; i < n; i++) {
        indices[i] = i;
    }

    RANDOM_LOG("[random_sample]: Shuffling array indices to select %zu samples.", num_samples);
    for (size_t i = 0; i < num_samples; i++) {
        size_t j = i + random_index(n - i);
        RANDOM_LOG("[random_sample]: Swapping indices[%zu] with indices[%zu]", i, j);

        size_t temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    RANDOM_LOG("[random_sample]: Copying selected samples to output array.");
    for (size_t i = 0; i < num_samples; i++) {
        memcpy((char *)samples + i * size, (char *)array + indices[i] * size, size);
        RANDOM_LOG("[random_sample]: Copied sample %zu from array index %zu", i, indices[i]);
    }

    free(indices);
    RANDOM_LOG("[random_sample]: Exiting random_sample.");
}


/**
 * @brief This function sets the state of the random number generator to the specified value.
 * 
 * @param state A pointer to the state value to set the random number generator to.
 *       if `state` is NULL, the function does nothing.
 */
void random_setstate(const unsigned int *state) {
    RANDOM_LOG("[random_setstate]: Entering random_setstate with state: %p", (void*)state);

    if (state != NULL) {
        rs_store(*state);
        srand(*state);
        RANDOM_LOG("[random_setstate]: State set to: %u", *state);
    } 
    else {
        RANDOM_LOG("[random_setstate]: Error: state is NULL in random_setstate.");
    }

    RANDOM_LOG("[random_setstate]: Exiting random_setstate");
}


/**
 * @brief This function retrieves the current state of the random number generator.
 * 
 * @param state A pointer to where the current state of the random number generator will be stored. 
 *       if `state` is NULL, the function does nothing.
 */
void random_getstate(unsigned int *state) {
    RANDOM_LOG("[random_getstate]: Entering random_getstate with state: %p", (void*)state);

    if (state != NULL) {
        *state = rs_load();
        RANDOM_LOG("[random_getstate]: Retrieved state: %u", *state);
    } 
    else {
        RANDOM_LOG("[random_getstate]: Error: state is NULL in random_getstate.");
    }

    RANDOM_LOG("[random_getstate]: Exiting random_getstate");
}


/**
 * @brief Generates a random double number based on the Gaussian (normal) distribution.
 *
 * @param mean The mean (mu) of the distribution.
 * @param stddev The standard deviation (sigma) of the distribution.
 * @return A random double number from the Gaussian distribution.
 *
 * @note The function uses the Box-Muller transform to generate a normally-distributed number.
 * 
 */
double random_gauss(double mean, double stddev) {
    RANDOM_LOG("[random_gauss]: Entering random_gauss with mean: %f, stddev: %f", mean, stddev);

    /* Marsaglia polar method. Each accepted pair holds two normal values, but
       the second is no longer cached: a static spare was shared by all
       threads and survived random_seed / random_setstate, so a reseeded
       stream (and gamma, beta, lognormal built on it) could start with a
       value left over from the old one. A call now depends only on the
       generator state. */
    double u, v, s;
    do {
        u = random_random() * 2.0 - 1.0;
        v = random_random() * 2.0 - 1.0;
        s = u * u + v * v;

        RANDOM_LOG("[random_gauss]: Generated u: %f, v: %f, s: %f", u, v, s);
    } while (s >= 1.0 || s == 0.0);

    s = sqrt(-2.0 * log(s) / s);

    double result = mean + stddev * (u * s);
    RANDOM_LOG("[random_gauss]: Exiting random_gauss with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the Exponential distribution.
 *
 * @param lambda The rate parameter (1/mean) of the distribution (must be greater than 0).
 * @return A random double number from the Exponential distribution.
 *         If `lambda` is less than or equal to 0, the function prints an error message
 *         and returns NAN.
 *
 * @note The function uses the inverse transform sampling method to produce the exponentially-distributed number.
 *       If `lambda` is less than or equal to 0, the function will print error message and return NAN.
 */
double random_expo(double lambda) {
    RANDOM_LOG("[random_expo]: Entering random_expo with lambda: %f", lambda);

    if (lambda <= 0.0) {
        RANDOM_LOG("[random_expo]: Error: lambda is less than or equal to 0 in random_expo.");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
        RANDOM_LOG("[random_expo]: Generated expo: %f", expo);
    } while (expo == 0.0);

    double result = -log(expo) / lambda;
    RANDOM_LOG("[random_expo]: Exiting random_expo with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the Log-normal distribution.
 *
 * @param mean The mean parameter of the underlying normal distribution.
 * @param stddev The standard deviation parameter of the underlying normal distribution.
 * @return A random double number from the Log-normal distribution.
 * 
 */
double random_lognormal(double mean, double stddev) {
    RANDOM_LOG("[random_lognormal]: Entering random_lognormal with mean: %f, stddev: %f", mean, stddev);

    double normal_value = random_gauss(mean, stddev);
    RANDOM_LOG("[random_lognormal]: Generated normal_value: %f from underlying normal distribution", normal_value);

    double result = exp(normal_value);
    RANDOM_LOG("[random_lognormal]: Exiting random_lognormal with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the Gamma distribution.
 *
 * @param shape The shape parameter of the distribution (must be greater than 0).
 * @param scale The scale parameter of the distribution (must be greater than 0).
 * @return A random double number from the Gamma distribution.
 *         If `shape` or `scale` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 *
 * @note The function uses different methods depending on the value of `shape`:
 *       - For `shape` < 1, Johnk's generator is used.
 *       - For `shape` >= 1, Marsaglia and Tsang's method is used.
 */
double random_gamma(double shape, double scale) {
    RANDOM_LOG("[random_gamma]: Entering random_gamma with shape: %f, scale: %f", shape, scale);

    if (shape <= 0.0 || scale <= 0.0) {
        RANDOM_LOG("[random_gamma]: Error: shape or scale is less than or equal to 0 in random_gamma.");
        return NAN;
    }

    if (shape < 1.0) {
        RANDOM_LOG("[random_gamma]: Using Johnk's generator for shape < 1");
        double expo;

        do {
            expo = random_random();
            RANDOM_LOG("[random_gamma]: Generated expo: %f", expo);
        } while (expo == 0.0);

        double result = random_gamma(1.0 + shape, scale) * pow(expo, 1.0 / shape);
        RANDOM_LOG("[random_gamma]: Exiting random_gamma (Johnk's) with result: %f", result);
        return result;
    }

    RANDOM_LOG("[random_gamma]: Using Marsaglia and Tsang's method for shape >= 1");
    double d = shape - 1.0 / 3.0;
    double c = 1.0 / sqrt(9.0 * d);
    double v;

    while (1) {
        double u = random_random();
        double z = random_gauss(0.0, 1.0);
        v = pow(1.0 + c * z, 3);
        RANDOM_LOG("[random_gamma]: Generated u: %f, z: %f, v: %f", u, z, v);
        if (u < 1.0 - 0.0331 * (z * z) * (z * z)) {
            RANDOM_LOG("[random_gamma]: Break condition met in Marsaglia and Tsang's method (u condition).");
            break;
        }
        if (log(u) < 0.5 * z * z + d * (1.0 - v + log(v))) {
            RANDOM_LOG("[random_gamma]: Break condition met in Marsaglia and Tsang's method (log(u) condition).");
            break;
        }
    }

    double result = d * v * scale;
    RANDOM_LOG("[random_gamma]: Exiting random_gamma (Marsaglia and Tsang's) with result: %f", result);
    return result;
}


/**
 * @brief Generates a random double number based on the Beta distribution.
 *
 * @param alpha The shape1 parameter of the distribution (must be greater than 0).
 * @param beta The shape2 parameter of the distribution (must be greater than 0).
 * @return A random double number from the Beta distribution.
 *         If `alpha` or `beta` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 */
double random_beta(double alpha, double beta) {
    RANDOM_LOG("[random_beta]: Entering random_beta with alpha: %f, beta: %f", alpha, beta);

    if (alpha <= 0.0 || beta <= 0.0) {
        RANDOM_LOG("[random_beta]: Error: alpha or beta is less than or equal to 0 in random_beta.");
        return NAN;
    }

    double x = random_gamma(alpha, 1.0);
    double y = random_gamma(beta, 1.0);
    RANDOM_LOG("[random_beta]: Generated x: %f, y: %f for random_beta", x, y);

    double result = x / (x + y);
    RANDOM_LOG("[random_beta]: Exiting random_beta with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the Pareto distribution.
 * which is a continuous probability distribution.
 *
 * @param shape The shape parameter (a) of the distribution (must be greater than 0).
 * @param scale The scale parameter (m) of the distribution (must be greater than 0).
 * @return A random double number from the Pareto distribution.
 *         If `shape` or `scale` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 */
double random_pareto(double shape, double scale) {
    RANDOM_LOG("[random_pareto]: Entering random_pareto with shape: %f, scale: %f", shape, scale);

    if (shape <= 0.0 || scale <= 0.0) {
        RANDOM_LOG("[random_pareto]: Error: shape or scale is less than or equal to 0 in random_pareto.");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
        RANDOM_LOG("[random_pareto]: Generated expo: %f", expo);
    } while (expo == 0.0);

    double result = scale * pow((1.0 / expo), (1.0 / shape));
    RANDOM_LOG("[random_pareto]: Exiting random_pareto with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the Weibull distribution.
 * which is a continuous probability distribution.
 *
 * @param shape The shape parameter (a) of the distribution (must be greater than 0).
 * @param scale The scale parameter (λ) of the distribution (must be greater than 0).
 * @return A random double number from the Weibull distribution.
 *         If `shape` or `scale` is less than or equal to 0, the function prints an error message
 *         and returns NaN.
 */
double random_weibull(double shape, double scale) {
    RANDOM_LOG("[random_weibull]: Entering random_weibull with shape: %f, scale: %f", shape, scale);

    if (shape <= 0.0 || scale <= 0.0) {
        RANDOM_LOG("[random_weibull]: Error: shape or scale is less than or equal to 0 in random_weibull.");
        return NAN;
    }

    double expo;
    do {
        expo = random_random();
    } while (expo == 0.0);

    double result = scale * pow(-log(expo), 1.0 / shape);
    RANDOM_LOG("[random_weibull]: Exiting random_weibull with result: %f", result);

    return result;
}


/**
 * @brief Generates a random double number based on the von Mises distribution.
 * which is a continuous probability distribution on the circle.
 *
 * @param mu The mean direction of the distribution (in radians).
 * @param kappa The concentration parameter (must be greater than 0). If `kappa` is less than or equal to 0, 
 *              the function prints an error message and returns NAN.
 * @return A random double number from the von Mises distribution.
 *         If `kappa` is less than or equal to 0, the function prints an error message and returns NAN.
 *         
 */
double random_vonmises(double mu, double kappa) {
    RANDOM_LOG("[random_vonmises]: Entering random_vonmises with mu: %f, kappa: %f", mu, kappa);
    if (!(kappa > 0.0)) {   /* also rejects NaN, which would never leave the loop */
        RANDOM_LOG("[random_vonmises]: Error: kappa is less than or equal to 0 in random_vonmises.");
        return NAN;
    }

    const double tau = 2 * M_PI;
    double theta;

    if (kappa <= 1e-6) {
        /* Practically uniform on the circle. */
        theta = tau * random_random();
    }
    else {
        /* Best & Fisher (1979) rejection sampler, as used by CPython's
           random.vonmisesvariate. */
        double s = 0.5 / kappa;
        double r = s + sqrt(1.0 + s * s);
        double z, d, u2;

        do {
            z = cos(M_PI * random_random());
            d = z / (r + z);
            u2 = random_random();
        } while (!(u2 < 1.0 - d * d || u2 <= (1.0 - d) * exp(d)));

        double q = 1.0 / r;
        double f = (q + z) / (1.0 + q * z);
        if (f > 1.0) {
            f = 1.0;     /* rounding guard for acos */
        }
        else if (f < -1.0) {
            f = -1.0;
        }
        /* The angle is mirrored around mu, not around mu + pi/2. */
        theta = (random_random() > 0.5) ? mu + acos(f) : mu - acos(f);
    }

    /* Python-style modulo: the result is in [0, 2*pi) even for negative mu. */
    double result = fmod(theta, tau);
    if (result < 0.0) {
        result += tau;
    }
    if (result >= tau) {
        result = 0.0;
    }
    RANDOM_LOG("[random_vonmises]: Exiting random_vonmises with result: %f", result);

    return result;
}
