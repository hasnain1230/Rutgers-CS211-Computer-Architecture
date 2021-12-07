#include <stdio.h>
#include <ctype.h>
#include <string.h>

void makeUpper(char* inString) { // Will also add two exclamation marks too whether you like it or not. So, deal with it.
    for (int x = 0; x < strlen(inString); x++) {
        char ch = inString[x];
        inString[x] = (char) toupper(ch);
    }
    strcat(inString, "!!");
}

int main(int argc, char *argv[]) {
    if (argc == 1 || (argc == 2 && strcmp(argv[1], "") == 0)) {
        return 0;
    }

    makeUpper(argv[1]);
    printf("%s\n", argv[1]);

    return 0;
}