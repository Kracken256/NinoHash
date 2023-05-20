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

#include "ninohash.h"

#define NINO_NUM_ROUNDS 128

#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (64 - (b))))

#define permute_box1(a, b, c, d)                  \
    *d ^= (((*a) << 32) & ~((*d) >> 32)) ^ ~(*a); \
    *c ^= (((*b) << 48) & ~((*c) >> 16)) ^ ~(*b); \
    *b ^= (((*c) << 32) & ~((*b) >> 32)) ^ ~(*c); \
    *a ^= (((*d) << 16) & ~((*a) >> 48)) ^ ~(*d);

#define permute_box2(a, b, c, d)                                      \
    *a ^= ROTRIGHT(((*a ^ *b) & ~*c), 20) << (ROTRIGHT(*d, 12) % 14); \
    *b ^= ROTRIGHT(((*b ^ *c) & ~*d), 26) << (ROTRIGHT(*a, 17) % 14); \
    *c ^= ROTRIGHT(((*c ^ *d) & ~*a), 15) << (ROTRIGHT(*b, 29) % 14); \
    *d ^= ROTRIGHT(((*d ^ *a) & ~*b), 37) << (ROTRIGHT(*c, 47) % 14);

#define permute_box3(a, b, c, d)                               \
    *a ^= ROTRIGHT(((*a & *b) ^ ~*c), (*d % 64)) >> (*a % 16); \
    *b ^= ROTRIGHT(((*b & *c) ^ ~*d), (*c % 63)) >> (*b % 17); \
    *c ^= ROTRIGHT(((*c & *d) ^ ~*a), (*b % 62)) >> (*c % 18); \
    *d ^= ROTRIGHT(((*d & *a) ^ ~*b), (*a % 61)) >> (*d % 19);

// process data in 256 bit chunks
void nino256_init(nino256_CTX *ctx)
{
    memset(ctx, 0, sizeof(nino256_CTX));
    ctx->state[0] = 0x6a09e667f3bcc908;
    ctx->state[1] = 0xbb67ae8584caa73b;
    ctx->state[2] = 0x3c6ef372fe94f82b;
    ctx->state[3] = 0xa54ff53a5f1d36f1;
    ctx->round_count = NINO_NUM_ROUNDS;
}

// process data in 256 bit chunks
static inline void nino256_transform(nino256_CTX *ctx, uint8_t *chunk)
{
    ctx->state[0] ^= *((uint64_t *)(chunk + 0));
    ctx->state[1] ^= *((uint64_t *)(chunk + 8));
    ctx->state[2] ^= *((uint64_t *)(chunk + 16));
    ctx->state[3] ^= *((uint64_t *)(chunk + 24));

    // permute the state
    permute_box1(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
    permute_box2(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
    permute_box3(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
}

void nino256_update(nino256_CTX *ctx, const uint8_t *buffer, size_t length)
{
    // loop through the buffer in 512 bit chunks
    for (size_t i = 0; i < length; i += 32)
    {
        // if there is less than 512 bits left
        if (length - i < 32)
        {
            // pad the remaining bytes with zeros
            uint8_t chunk[32] = {0};
            memcpy(chunk, buffer + i, length - i);
            memcpy(chunk + 28, &length, 4);
            nino256_transform(ctx, chunk);
        }
        else
        {
            // process the next 256 bytes
            nino256_transform(ctx, buffer + i);
        }
    }
}

void nino256_final(nino256_CTX *ctx)
{
    // Perform the rounds here [for speed]
    for (uint16_t j = 0; j < ctx->round_count; j++)
    {
        permute_box1(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
        permute_box2(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
        permute_box3(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
    }
    memcpy(ctx->digest, ctx->state, 32);
    // clear the state
    memset(ctx->state, 0, 32);
}

void nino256sum(const uint8_t *buffer, size_t length, uint8_t *hash)
{
    nino256_CTX ctx;
    nino256_init(&ctx);
    nino256_update(&ctx, buffer, length);
    nino256_final(&ctx);
    memcpy(hash, ctx.digest, 32);
}