/**
 * @author Amin Tahmasebi
 * @date 2026
 * @class UUID
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in uuid.c (file-level convention).
 *
 * Universally Unique Identifiers per RFC 9562 (the spec that obsoletes
 * RFC 4122), with an API modelled on Python's `uuid` module.
 *
 * A `UUID` is a 16-byte value type (big-endian field order, a.k.a.
 * network byte order). Generators return one by value; formatting writes
 * into a caller-supplied buffer. The whole module is allocation-free, so
 * there is nothing to leak.
 *
 * Implemented versions:
 *   - v4: random                       (uuid_v4)
 *   - v7: Unix-time-ordered + random   (uuid_v7)   — recommended for new keys
 *   - v3: name-based, MD5              (uuid_v3)
 *   - v5: name-based, SHA-1            (uuid_v5)
 * Plus the Nil and Max special UUIDs and the four predefined namespaces.
 *
 * Dependencies: only the c_std `secrets` module (its cross-platform CSPRNG
 * drives v4/v7). MD5 and SHA-1 are embedded, so there is no OpenSSL link.
 */

 
#ifndef UUID_H_
#define UUID_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define UUID_LOGGING_ENABLE */

#ifdef UUID_LOGGING_ENABLE
    #define UUID_LOG(...) \
        do { fprintf(stderr, "[UUID LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
    #define UUID_LOG(...) do { } while (0)
#endif


/* Buffer sizes for the formatting helpers (include room for the NUL). */
#define UUID_STR_LEN  37    /* "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" + NUL  */
#define UUID_URN_LEN  46    /* "urn:uuid:" + 36 + NUL                        */


/* A UUID is 16 bytes in big-endian field order. Value semantics: copy it
 * freely, compare it with uuid_equal / uuid_compare.
 *
 * The type is Uuid; `UUID` is an alias for it. Windows declares its own
 * UUID (<rpcdce.h>, pulled in by <windows.h>) behind the UUID_DEFINED guard,
 * and the two used to collide in every translation unit that had both:
 *  - <windows.h> first: UUID stays Windows' GUID, no alias is made; use
 *    Uuid for this library there.
 *  - this header first: the alias is made and UUID_DEFINED is set, so a
 *    later <windows.h> skips its own; its RPC functions (UuidCreate,
 *    UuidToString, ...) then take a Uuid* (the same 16-byte layout).
 * Define CSTD_UUID_NO_ALIAS to never declare `UUID`. */
typedef struct Uuid {
    unsigned char bytes[16];
} Uuid;

#if !defined(CSTD_UUID_NO_ALIAS) && !defined(UUID_DEFINED)
    #define UUID_DEFINED
    typedef Uuid UUID;
#endif


/* RFC 9562 variant (the top bits of octet 8). */
typedef enum {
    UUID_VARIANT_NCS = 0,    /* 0xx  — reserved (NCS backward compatibility) */
    UUID_VARIANT_RFC4122,    /* 10x  — the standard RFC 4122 / 9562 variant  */
    UUID_VARIANT_MICROSOFT,  /* 110  — reserved (Microsoft)                  */
    UUID_VARIANT_FUTURE      /* 111  — reserved for future definition        */
} UuidVariant;


/* ------------------------------------------------------------------ */
/* Generators                                                         */
/* ------------------------------------------------------------------ */

Uuid         uuid_v4               (void);
Uuid         uuid_v7               (void);
Uuid         uuid_v3               (Uuid namespace_id, const char* name);
Uuid         uuid_v5               (Uuid namespace_id, const char* name);


/* ------------------------------------------------------------------ */
/* Special values                                                     */
/* ------------------------------------------------------------------ */

Uuid         uuid_nil              (void);
Uuid         uuid_max              (void);


/* ------------------------------------------------------------------  */
/* Predefined namespaces (RFC 9562 Appendix C) — for uuid_v3 / uuid_v5 */
/* ------------------------------------------------------------------  */

Uuid         uuid_namespace_dns    (void);
Uuid         uuid_namespace_url    (void);
Uuid         uuid_namespace_oid    (void);
Uuid         uuid_namespace_x500   (void);


/* ------------------------------------------------------------------  */
/* Formatting / parsing (no allocation; caller-supplied buffers)       */
/* ------------------------------------------------------------------  */

void         uuid_to_string        (Uuid u, char out[UUID_STR_LEN]);
void         uuid_to_urn           (Uuid u, char out[UUID_URN_LEN]);
bool         uuid_from_string      (const char* str, Uuid* out);

void         uuid_to_bytes         (Uuid u, unsigned char out[16]);
Uuid         uuid_from_bytes       (const unsigned char in[16]);


/* ------------------------------------------------------------------ */
/* Inspection                                                         */
/* ------------------------------------------------------------------ */

int                uuid_version        (Uuid u);
UuidVariant        cstd_uuid_variant   (Uuid u);
bool               uuid_is_nil         (Uuid u);
bool               uuid_is_max         (Uuid u);
bool               uuid_equal          (Uuid a, Uuid b);
int                cstd_uuid_compare   (Uuid a, Uuid b);
unsigned long long uuid_v7_timestamp_ms(Uuid u);

/* Linux libuuid (util-linux) exports uuid_compare and uuid_variant with other
 * signatures, so the linked symbols are cstd_uuid_compare / cstd_uuid_variant
 * and the familiar names are macros: a program linking libcstd and libuuid
 * no longer gets one definition interposing the other. Define
 * CSTD_UUID_NO_FUNC_ALIAS (e.g. in a file that also includes libuuid's
 * header) to drop the macros and call the cstd_ names directly. */
#if !defined(CSTD_UUID_NO_FUNC_ALIAS)
    #define uuid_compare cstd_uuid_compare
    #define uuid_variant cstd_uuid_variant
#endif


#ifdef __cplusplus
}
#endif

#endif
