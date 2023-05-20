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

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (64 - (b))))
#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (64 - (b))))

static inline void permute_box1(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
{
    *d ^= (((*a) << 32) ^ ((*d) >> 32)) + (*a) + 1;
    *c ^= (((*b) << 48) ^ ((*c) >> 16)) + (*b) + 1;
    *b ^= (((*c) << 32) ^ ((*b) >> 32)) + (*c) + 1;
    *a ^= (((*d) << 16) ^ ((*a) >> 48)) + (*d) + 1;

    // Additional mixing
    *a += *b & ~*c;
    *b += *c & ~*d;
    *c += *d & ~*a;
    *d += *a & ~*b;
}

static inline void permute_box2(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
{
    *a ^= ((*a ^ *b) ^ ~*c) << (*d % 7);
    *b ^= ((*b ^ *c) ^ ~*d) << (*a % 7);
    *c ^= ((*c ^ *d) ^ ~*a) << (*b % 7);
    *d ^= ((*d ^ *a) ^ ~*b) << (*c % 7);

    // Additional mixing
    *a = ROTLEFT(*a, 17) ^ ROTRIGHT(*c, 7);
    *b = ROTLEFT(*b, 29) ^ ROTRIGHT(*d, 3);
    *c = ROTLEFT(*c, 47) ^ ROTRIGHT(*b, 13);
    *d = ROTLEFT(*d, 59) ^ ROTRIGHT(*a, 23);
}

static inline void permute_box3(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
{
    *a ^= ROTRIGHT(((*a ^ *b) ^ ~*c), (*d % 64)) >> (*a % 16);
    *b ^= ROTRIGHT(((*b ^ *c) ^ ~*d), (*c % 64)) >> (*b % 16);
    *c ^= ROTRIGHT(((*c ^ *d) ^ ~*a), (*b % 64)) >> (*c % 16);
    *d ^= ROTRIGHT(((*d ^ *a) ^ ~*b), (*a % 64)) >> (*d % 16);

    // Additional mixing
    *a = (*a ^ *b) + (*c ^ *d);
    *b = (*b ^ *c) + (*d ^ *a);
    *c = (*c ^ *d) + (*a ^ *b);
    *d = (*d ^ *a) + (*b ^ *c);
}

int main()
{
    uint64_t state_1 = 0x27b5af54b4339563;
    uint64_t state_2 = 0x2f871e900df1b581;
    uint64_t state_3 = 0x4cd6eb4767103113;
    uint64_t state_4 = 0xb66a3dd079f530b3;
    FILE *output = fopen("csprng.bin", "wb");
    const uint16_t rounds = 2;
    for (long iteration = 0; iteration < (75000000000 / 32); iteration++)
    {
        // Perform the rounds
        for (uint16_t j = 0; j < rounds; j++)
        {
            permute_box1(&state_1, &state_2, &state_3, &state_4);
            permute_box2(&state_1, &state_2, &state_3, &state_4);
            permute_box3(&state_1, &state_2, &state_3, &state_4);
        }

        uint8_t digest[32];

        *((uint64_t *)(digest + 0)) = state_1;
        *((uint64_t *)(digest + 8)) = state_2;
        *((uint64_t *)(digest + 16)) = state_3;
        *((uint64_t *)(digest + 24)) = state_4;

        fwrite(digest, 1, 32, output);
    }
    fclose(output);
}