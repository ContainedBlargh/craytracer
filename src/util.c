#include "util.h"
#include "fail.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief Checks if a char exists in a string.
 *
 * @param string The string to check.
 * @param character The char to look for.
 *
 * @return Whether the char is contained within the string.
 */
bool char_in_string(char* string, char character) {
    char* char_pos = strchr(string, character);
    return char_pos != NULL;
}

void char_replace(char* string, char before, char after) {
    char* char_pos = strchr(string, before);
    if (char_pos == NULL) {
        return;
    }
    while (char_pos) {
        *char_pos = after;
        char_pos = strchr(char_pos + 1, before);
    }
}

char *read_file(char *path)
{
    FILE *fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *contents = malloc(fsize + 1);
    fread(contents, fsize, sizeof(char), fp);
    fclose(fp);
    contents[fsize] = '\0';
    return contents;
}
