#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Stack {
    size_t size;
    int topStackIndex;
    char *data;
};

struct Stack* makeStack(unsigned long int capacity) {
    size_t stackSize = sizeof(struct Stack);
    struct Stack* stack = (struct Stack*) malloc(stackSize); // Allocate
    stack->size = capacity;
    stack->topStackIndex = 0;
    stack->data = (char*) malloc((stack->size * sizeof(char)) + 1); // Plus 1 because it's very possible we might get a terminator byte here. Not likely, but better to be safe than sorry.

    return stack;
}

int isEmpty(struct Stack* s) {
    return s->topStackIndex == 0; // If top stack is still the default zero, no elements were added.
}

int isFull(struct Stack* s) {
    return s->topStackIndex == s->size;
}

void push(struct Stack* s, char data) {
    if (isFull(s)) {
        return;
    } else {
        s->data[(s->topStackIndex)] = data;
        s->topStackIndex++;
    }
}

char pop(struct Stack* s) {
    if (isEmpty(s)) {
        return -1;
    } else {
        s->topStackIndex--;
        return s->data[s->topStackIndex];
    }
}

char findOpposite(char parentheses) {
    switch (parentheses) {
        case ')':
            return '(';
        case ']':
            return '[';
        case '}':
            return '{';
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        default:
            perror("Something went terribly wrong! You entered an invalid parentheses! Fix it!!");
            return -1;
    }
}

int main(int argc, char* argv[]) {
    size_t strSize = strlen(argv[1]);
    struct Stack* parStack = makeStack(strSize);

    for (int x = 0; x < strlen(argv[1]); x++) {
        if (argv[1][x] == '(' || argv[1][x] == '[' || argv[1][x] == '{') {
            push(parStack, argv[1][x]);
        } else if (argv[1][x] == ')' || argv[1][x] == ']' || argv[1][x] == '}') {
            char opposite = findOpposite(argv[1][x]);
            char poppedPar = pop(parStack);

            if (opposite == poppedPar) {
                continue;
            } else {
                printf("%d: %c", x, argv[1][x]);

                free(parStack->data);
                free(parStack);

                return EXIT_FAILURE;
            }
        }
    }

    if (!isEmpty(parStack)) {
        printf("open: ");
        while (!isEmpty(parStack)) {
            printf("%c", findOpposite(pop(parStack)));
        }

        free(parStack->data);
        free(parStack);

        return EXIT_FAILURE;
    }

    free(parStack->data);
    free(parStack);

    return 0;
}