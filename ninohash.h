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
    uint16_t round_count;
    uint8_t digest[32];
    uint64_t state[4];
} nino256_CTX;

void nino256_init(nino256_CTX *ctx);
void nino256_update(nino256_CTX *ctx, const uint8_t *buffer, size_t length);
void nino256_final(nino256_CTX *ctx);

void nino256sum(const uint8_t *buffer, size_t length, uint8_t *hash);