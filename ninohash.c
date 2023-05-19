/*
 * Ultra TOP SECRET crypto algorithm.
 * Yes. I rolled my own crypto.
 * With 8 rounds, this will be okay.
 * Please note that there is no math basis for why this is how it is. I just made it up.
 * It passed all Dieharder with flying colors, so it's probably not complete crap?
 * I'm not a cryptographer. I'm just a guy who likes to play with numbers. It may or may not be secure enough to last.
 * This could be faster. I'm trying to figure out how to make it faster other than lowering the number of rounds or inventing an ASIC.
 [This was Helixrat from my other project. Changed the name to Nino. Changed the state initialization to be more legit.]

 */
#include "ninohash.h"

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

void nino256sum(const uint8_t *const buffer, uint32_t length, uint16_t rounds, uint8_t *hash)
{
    uint64_t state_1 = rounds;
    uint64_t state_2 = rounds;
    uint64_t state_3 = rounds;
    uint64_t state_4 = rounds;

    // Prepare states (for case length == 0) lets look legit
    permute_box1(&state_1, &state_2, &state_3, &state_4);
    permute_box2(&state_1, &state_2, &state_3, &state_4);
    permute_box3(&state_1, &state_2, &state_3, &state_4);
    // If there is any data...
    if (length > 0)
    {
        // Iterate over the buffer in 32 byte chunks
        for (uint32_t i = 0; i < (length - (length % 32)); i += 32)
        {
            // Load the next 32 bytes into the state
            state_1 ^= *((uint64_t *)(buffer + i));
            state_2 ^= *((uint64_t *)(buffer + i + 8));
            state_3 ^= *((uint64_t *)(buffer + i + 16));
            state_4 ^= *((uint64_t *)(buffer + i + 24));

            // Perform the rounds
            for (uint16_t j = 0; j < rounds; j++)
            {
                permute_box1(&state_1, &state_2, &state_3, &state_4);
                permute_box2(&state_1, &state_2, &state_3, &state_4);
                permute_box3(&state_1, &state_2, &state_3, &state_4);
            }
        }

        // Process the remaining bytes
        if (length % 32 != 0)
        {
            uint8_t remainingBytes = length % 32;
            // Load the remaining bytes into the state
            for (uint8_t i = 0; i < remainingBytes; i++)
            {
                *((uint8_t *)(&state_1) + ((i + 0) % 8)) ^= buffer[length - remainingBytes + i];
                *((uint8_t *)(&state_2) + ((i + 1) % 8)) ^= buffer[length - remainingBytes + i];
                *((uint8_t *)(&state_3) + ((i + 2) % 8)) ^= buffer[length - remainingBytes + i];
                *((uint8_t *)(&state_4) + ((i + 3) % 8)) ^= buffer[length - remainingBytes + i];
            }
            // Perform the rounds
            for (uint16_t j = 0; j < rounds; j++)
            {
                permute_box1(&state_1, &state_2, &state_3, &state_4);
                permute_box2(&state_1, &state_2, &state_3, &state_4);
                permute_box3(&state_1, &state_2, &state_3, &state_4);
            }
        }
    }

    *((uint64_t *)(hash + 0)) = state_1;
    *((uint64_t *)(hash + 8)) = state_2;
    *((uint64_t *)(hash + 16)) = state_3;
    *((uint64_t *)(hash + 24)) = state_4;
}