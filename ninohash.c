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

#define NINO_NUM_ENDROUNDS 128

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
    // These are the initial state values (taken from SHA2-512).
    // 0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC
    ctx->state[0] = 0x428A2F98D728AE22;
    ctx->state[1] = 0x7137449123EF65CD;
    ctx->state[2] = 0xB5C0FBCFEC4D3B2F;
    ctx->state[3] = 0xE9B5DBA58189DBBC;
}

void nino256_init_complex(nino256_CTX *ctx, const uint8_t *const secret_key, size_t secret_key_length)
{
    nino256_init(ctx);
    nino256_update(ctx, secret_key, secret_key_length);

    // Note the state stays in the ctx
    // The secret key is now the intial state.
    nino256_final(ctx);
}

// process data in 256 bit chunks
static inline void nino256_transform(nino256_CTX *ctx)
{
    ctx->state[0] ^= *((uint64_t *)(ctx->buffer + 0));
    ctx->state[1] ^= *((uint64_t *)(ctx->buffer + 8));
    ctx->state[2] ^= *((uint64_t *)(ctx->buffer + 16));
    ctx->state[3] ^= *((uint64_t *)(ctx->buffer + 24));

    // permute the state
    permute_box1(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
    permute_box2(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
    permute_box3(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
}

void nino256_update(nino256_CTX *ctx, const uint8_t *const buffer, size_t length)
{
    // loop through the buffer in 512 bit chunks
    for (size_t i = 0; i < length; i += 32)
    {
        // if there is less than 512 bits left
        if (length - i < 32)
        {
            // pad the remaining bytes with zeros
            memset(ctx->buffer, 0, 32);
            memcpy(ctx->buffer, buffer + i, length - i);
            memcpy(ctx->buffer + 28, &length, 4);
            nino256_transform(ctx);
        }
        else
        {
            memcpy(ctx->buffer, buffer + i, 32);
            // process the next 256 bytes
            nino256_transform(ctx);
        }
    }
}

void nino256_final(nino256_CTX *ctx)
{
    // Perform the rounds here [for speed]
    for (uint16_t j = 0; j < NINO_NUM_ENDROUNDS; j++)
    {
        permute_box1(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
        permute_box2(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
        permute_box3(&ctx->state[0], &ctx->state[1], &ctx->state[2], &ctx->state[3]);
    }
    memcpy(ctx->digest, ctx->state, 32);
}

void nino256sum(const uint8_t *const buffer, size_t length, uint8_t *hash)
{
    nino256_CTX ctx;
    nino256_init(&ctx);
    nino256_update(&ctx, buffer, length);
    nino256_final(&ctx);
    memcpy(hash, ctx.digest, 32);
}