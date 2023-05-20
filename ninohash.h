/*
    # ------------------------------------------------------------------------------- #
    #    ___       ___       ___       ___       ___       ___       ___       ___    #
    #   /\__\     /\  \     /\__\     /\  \     /\__\     /\  \     /\  \     /\__\   #
    #  /:| _|_   _\:\  \   /:| _|_   /::\  \   /:/__/_   /::\  \   /::\  \   /:/__/_  #
    # /::|/\__\ /\/::\__\ /::|/\__\ /:/\:\__\ /::\/\__\ /::\:\__\ /\:\:\__\ /::\/\__\ #
    # \/|::/  / \::/\/__/ \/|::/  / \:\/:/  / \/\::/  / \/\::/  / \:\:\/__/ \/\::/  / #
    #   |:/  /   \:\__\     |:/  /   \::/  /    /:/  /    /:/  /   \::/  /    /:/  /  #
    #   \/__/     \/__/     \/__/     \/__/     \/__/     \/__/     \/__/     \/__/   #
    #                                                                                 #
    #                             Created by: Wesley Jones                            #
    #                                                                                 #
    # ------------------------------------------------------------------------------- #
*/

#pragma once

#include <stdint.h>
#include <string.h>

// Better safe then sorry.
#define NINO_DIGEST_LENGTH 32

typedef struct nino256_CTX
{
    uint8_t buffer[32];
    uint8_t digest[32];
    uint64_t state[4];
} nino256_CTX;

// Simple context initialization.
void nino256_init(nino256_CTX *ctx);

// Initialize with a secret key for HMAC.
void nino256_init_complex(nino256_CTX *ctx, const uint8_t *const secret_key, size_t secret_key_length);

// Update the context with a buffer.
void nino256_update(nino256_CTX *ctx, const uint8_t *const buffer, size_t length);

// Finalize the context and write the digest to the digest buffer in ctx.
void nino256_final(nino256_CTX *ctx);

// Easy to use function to hash a buffer. No key.
void nino256sum(const uint8_t *const buffer, size_t length, uint8_t *hash);