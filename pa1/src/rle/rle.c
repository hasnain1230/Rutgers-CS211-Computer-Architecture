#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void compressString(const char* initialString, char* compressedString, size_t strLength) {
    char ptr = initialString[0];
    int count = 0;
    size_t size = (2 * (strlen(initialString))) + 1;
    char *temp = (char *) malloc(size);

    for (int x = 0; x < strLength; x++) {
        if (ptr == initialString[x]) {
            count++;
        } else {
            sprintf(temp, "%c%d", ptr, count);
            strcat(compressedString, temp);
            ptr = initialString[x];
            x--;
            count = 0;
        }
    }

    if (count != 0) {
        sprintf(temp, "%c%d", ptr, count);
        strcat(compressedString, temp);
    }

    free(temp);
}


int isNumeric(char* encodedString) {
    for (int x = 0; x < strlen(encodedString); x++) {
        if (isdigit(encodedString[x]) != 0) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        puts("One argument only. I don't like you.");
    }


    if (isNumeric(argv[1]) == 1) {
        puts("error");
        return 0;
    }

    size_t size = (2 * (strlen(argv[1]))) + 1;
    char* compressedString = (char *) malloc(size);

    compressedString = strcpy(compressedString, "");

    size_t inputLength = (int) strlen(argv[1]);
    compressString(argv[1], compressedString, inputLength);

    compressedString = realloc(compressedString, strlen(compressedString) + 1);

    if (strlen(compressedString) > inputLength) {
        printf("%s", argv[1]);
    } else {
        printf("%s", compressedString);
    }


    free(compressedString);

    return 0;
}

