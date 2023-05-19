#include "ninohash.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    uint8_t hash[32];
    nino256sum((uint8_t *)argv[1], strlen(argv[1]), 8, hash);
    // convert to hex
    char hex[65];
    for (int i = 0; i < 32; i++)
        sprintf(hex + i * 2, "%02x", hash[i]);
    printf("%s\n", hex);

    return 0;
}