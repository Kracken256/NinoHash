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

    This is not suitable for large files.
*/

#include "ninohash.h"
#include <unistd.h>
#include <stdio.h>

#define READ_BUFFER_SIZE 1024 * 1024 * 2

int main(int argc, char **argv)
{
    char buffer[READ_BUFFER_SIZE];
    if (argc < 2)
    {
        // Read from stdin
        size_t length = 0;
        nino256_CTX ctx;
        nino256_init(&ctx);
        while ((length = read(STDIN_FILENO, buffer, READ_BUFFER_SIZE)) > 0)
        {
            nino256_update(&ctx, (uint8_t *)buffer, length);
        }
        nino256_final(&ctx);
        // convert to hex
        char hex[65];
        for (int i = 0; i < 32; i++)
            sprintf(hex + i * 2, "%02x", ctx.digest[i]);
        printf("%s\n", hex);
        return 0;
    }

    // Read from file
    FILE *file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("Error opening file\n");
        return 1;
    }
    size_t length = 0;
    nino256_CTX ctx;
    nino256_init(&ctx);
    while ((length = fread(buffer, 1, READ_BUFFER_SIZE, file)) > 0)
    {
        nino256_update(&ctx, (uint8_t *)buffer, length);
    }
    nino256_final(&ctx);
    // convert to hex
    char hex[65];
    for (int i = 0; i < 32; i++)
        sprintf(hex + i * 2, "%02x", ctx.digest[i]);
    printf("%s  %s\n", hex, argv[1]);

    return 0;
}