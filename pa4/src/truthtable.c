#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

enum KIND {AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER};

struct Data {
    int inputSize;
    int outputSize;
    int comboSize; // Amount of combinations?
    int allVariablesSize;
    int gatesSize; // All things with size should be size_t, but that would affect padding of struct. Can be fixed later, however.
    int *input;
    int *output;
    int *truthValue;
    int **gates;
    char **allVariables; // Ideally, creating a hash map would be smarter, but for now, we can create a pseudomap like this. (Looking back at this later, it would have been much easier too; but the time constraint forced me to stick with my current model.
};

struct Data *data;

void printTruthTable();

enum KIND convertStringToEnum(enum KIND gate, char readString[17]) {
    if (strcmp(readString, "AND") == 0) {
        return gate = AND;
    } else if (strcmp(readString, "OR") == 0) {
        return gate = OR;
    } else if (strcmp(readString, "NAND") == 0) {
        return gate = NAND;
    } else if (strcmp(readString, "NOR") == 0) {
        return gate = NOR;
    } else if (strcmp(readString, "XOR") == 0) {
        return gate = XOR;
    } else if (strcmp(readString, "NOT") == 0) {
        return gate = NOT;
    } else if (strcmp(readString, "PASS") == 0) {
        return gate = PASS;
    } else if (strcmp(readString, "DECODER") == 0) {
        return gate = DECODER;
    } else if (strcmp(readString, "MULTIPLEXER") == 0) {
        return gate = MULTIPLEXER;
    } else {
        perror("Invalid gate inputted!");
        exit(1);
    }
}

int checkStringInAllVariablesArray(char *readString) {
    for (int x = 0; x < data->allVariablesSize; x++) {
        if (strcmp(readString, data->allVariables[x]) == 0) {
            return x + 10; // Don't need an input / output array. Just maintain one array with all the variables.
        }
    }
    data->allVariables = (char **) realloc(data->allVariables, sizeof(char *) * (1 + data->allVariablesSize));
    data->allVariables[data->allVariablesSize] = (char *) malloc(sizeof(char) * 17);
    strcpy(data->allVariables[data->allVariablesSize], readString);
    data->allVariablesSize++;

    return (data->allVariablesSize - 1) + 10;
}

void appendToGates(FILE *file, char *readString) {
    data->gates = (int **) malloc(sizeof(int *));
    data->gatesSize = 1;

    int count = 0;

    while (fscanf(file, "%16s", readString) != EOF) {
        enum KIND gate = -1;
        gate = convertStringToEnum(gate, readString);
        assert(gate != -1);

        if (count != 0) {
            data->gates = (int **) realloc(data->gates, sizeof(int *) * ++(data->gatesSize));
        }

        if (gate == 5 || gate == 6) {
            data->gates[count] = (int *) malloc(sizeof(int) * 3);
            data->gates[count][0] = gate;
            fscanf(file, "%16s", readString);
            data->gates[count][1] = checkStringInAllVariablesArray(readString);
            fscanf(file, "%16s", readString);
            data->gates[count][2] = checkStringInAllVariablesArray(readString);
        } else if (gate >= 0 && gate <= 4) {
            data->gates[count] = (int *) malloc(sizeof(int) * 4);
            data->gates[count][0] = gate;
            fscanf(file, "%16s", readString);
            data->gates[count][1] = checkStringInAllVariablesArray(readString);
            fscanf(file, "%16s", readString);
            data->gates[count][2] = checkStringInAllVariablesArray(readString);
            fscanf(file, "%16s", readString);
            data->gates[count][3] = checkStringInAllVariablesArray(readString);
        } else if (gate == 7) {
            data->gates[count] = (int *) malloc(sizeof(int) * 2);
            data->gates[count][0] = gate;
            fscanf(file, "%d", &data->gates[count][1]);
            int totalRowSize = data->gates[count][1] + pow(2, data->gates[count][1]);
            data->gates[count] = (int *) realloc(data->gates[count], sizeof(int) * (2 + totalRowSize));

            for (int x = 0; x < totalRowSize; x++) {
                fscanf(file, "%16s", readString);
                data->gates[count][x + 2] = checkStringInAllVariablesArray(readString);
            }
        } else if (gate == 8) {
            data->gates[count] = (int *) malloc(sizeof(int) * 2);
            data->gates[count][0] = gate;
            fscanf(file, "%d", &data->gates[count][1]);

            int numOfInputs = pow(2, data->gates[count][1]);
            int totalSize = 2 + numOfInputs + data->gates[count][1] + 1;
            data->gates[count] = (int *) realloc(data->gates[count], sizeof(int) * totalSize);

            for (int x = 2; x < totalSize; x++) {
                fscanf(file, "%16s", readString);
                data->gates[count][x] = checkStringInAllVariablesArray(readString);
            }
        }
        count++;
    }
}


void processFile(FILE *file) {
    char readString[17]; // This is just for safety. I think C might be okay if this is 16, but I added one just for the null terminator.
    fscanf(file, "%16s", readString);
    data = (struct Data *) malloc(sizeof(struct Data)); // Malloc Call ~ Free later
    data->allVariablesSize = 0;
    int counter = 10; // 0 - 8 is reserved for the logic gate references. 9 is unused. Working with multiples of ten will be easier.

    if (strcmp(readString, "INPUT") == 0) {
        fscanf(file, "%d", &data->inputSize);
        data->comboSize = pow(2, data->inputSize);
        data->allVariables = (char **) malloc(sizeof(char *) * data->inputSize); // Malloc Call ~ Free Later

        data->input = (int *) malloc(sizeof(int) * data->inputSize);
        data->allVariablesSize += data->inputSize;

        for (int x = 0; x < data->inputSize; x++) {
            data->input[x] = counter;
            data->allVariables[x] = (char *) malloc(sizeof(char) * 17);
            fscanf(file, "%16s", data->allVariables[x]);
            counter++;
        }
    }

    fscanf(file, "%16s", readString);

    if (strcmp(readString, "OUTPUT") == 0) {
        fscanf(file, "%d", &data->outputSize);
        data->allVariablesSize += data->outputSize;
        data->allVariables = (char **) realloc(data->allVariables, sizeof(char *) * data->allVariablesSize);

        data->output = (int *) malloc(sizeof(int) * data->outputSize);

        for (int x = 0; x < data->outputSize; x++) {
            data->output[x] = counter;
            data->allVariables[x + (data->allVariablesSize - data->outputSize)] = (char *) malloc(sizeof(char) * 17);
            fscanf(file, "%16s", data->allVariables[x + (data->allVariablesSize - data->outputSize)]);
            counter++;
        }
    }

    appendToGates(file, readString);
}

int* bin(unsigned n, int bits) {
    int *binary = (int *) malloc(sizeof(int) * bits);
    int x = 0;

    /*
     * First, make space in i by shifting the bits the right amount to the left (since we want to go from left to right
     * If n & i is not zero, return 1. Otherwise, return 0
     * By dividing by two, we are moving that left most bit to the right once, and then rinsing and repeating.
     */

    for (int i = 1 << (bits - 1); i > 0; i = i / 2) {
        if (n & i) {
            binary[x] = 1;
            x++;
            continue;
        }
        binary[x] = 0;
        x++;
    }

    return binary;
}

int checkVariable(int variableNumber) {
    if (strcmp(data->allVariables[variableNumber - 10], "0") == 0) {
        return 0;
    } else if (strcmp(data->allVariables[variableNumber - 10], "1") == 0) {
        return 1;
    } else {
        return data->truthValue[variableNumber - 10];
    }
}

void fixGates(int startingIndex, int endingIndex, int row) {
    for (int x = startingIndex; x <= endingIndex; x++) {
        data->truthValue[data->gates[row][x] - 10] = checkVariable(data->gates[row][x]);
    }
}

void printTruthTable() {
    data->truthValue = (int *) malloc(sizeof(int) * data->allVariablesSize);

    for (int x = 0; x < data->comboSize; x++) {
        int *inputRows = bin(x, data->inputSize);

        for (int y = 0; y < data->inputSize; y++) {
            printf("%d ", inputRows[y]);
            data->truthValue[y] = inputRows[y];
        }

        for (int z = 0; z < data->gatesSize; z++) {
            switch (data->gates[z][0]) {
                case AND:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = checkVariable(data->gates[z][2]);

                    data->truthValue[data->gates[z][3] - 10] = data->truthValue[data->gates[z][1] - 10] && data->truthValue[data->gates[z][2] - 10];
                    break;
                case OR:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = checkVariable(data->gates[z][2]);
                    data->truthValue[data->gates[z][3] - 10] = data->truthValue[data->gates[z][1] - 10] || data->truthValue[data->gates[z][2] - 10];

                    break;
                case NAND:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = checkVariable(data->gates[z][2]);

                    data->truthValue[data->gates[z][3] - 10] = !(data->truthValue[data->gates[z][1] - 10] &&
                                                                 data->truthValue[data->gates[z][2] - 10]);
                    break;
                case NOR:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = checkVariable(data->gates[z][2]);

                    data->truthValue[data->gates[z][3] - 10] = !(data->truthValue[data->gates[z][1] - 10] ||
                                                                 data->truthValue[data->gates[z][2] - 10]);
                    break;
                case XOR:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = checkVariable(data->gates[z][2]);

                    data->truthValue[data->gates[z][3] - 10] = data->truthValue[data->gates[z][1] - 10] ^ data->truthValue[data->gates[z][2] - 10];

                    break;
                case NOT:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = !(data->truthValue[data->gates[z][1] - 10]);

                    break;
                case PASS:
                    data->truthValue[data->gates[z][1] - 10] = checkVariable(data->gates[z][1]);
                    data->truthValue[data->gates[z][2] - 10] = data->truthValue[data->gates[z][1] - 10];
                    break;
            }

            if (data->gates[z][0] == 7) {
                int decoderInputSize = data->gates[z][1];
                int decoderOutputSize = pow(2, decoderInputSize);

                fixGates(2, 2 + decoderInputSize - 1, z);

                for (int y = 0; y < decoderOutputSize; y++) {
                    int *binArrRows = bin(y, decoderInputSize);
                    int *truthValues = (int *) malloc(decoderInputSize * sizeof(int));

                    for (int k = 0; k < decoderInputSize; k++) {
                        if (binArrRows[k] == 0) {
                            truthValues[k] = !(data->truthValue[data->gates[z][2 + k] - 10]);
                        } else {
                            truthValues[k] = (data->truthValue[data->gates[z][2 + k] - 10]);
                        }
                    }

                    if (decoderInputSize == 1) {
                        data->truthValue[data->gates[z][2 + decoderInputSize + y] - 10] = truthValues[0] && truthValues[0];
                    } else {
                        int final = truthValues[0];

                        for (int j = 1; j < decoderInputSize; j++) {
                            final = final && truthValues[j];
                        }

                        data->truthValue[data->gates[z][2 + decoderInputSize + y] - 10] = final;
                    }

                    free(binArrRows);
                    free(truthValues);
                }
            } else if (data->gates[z][0] == 8) {
                int selectorBitSize = data->gates[z][1];
                int inputSize = pow(2, selectorBitSize);
                int *valuesAfterAnd = (int *) malloc(sizeof(int) * inputSize);
                int *selectorTruthValues = (int *) malloc(sizeof(int) * selectorBitSize);

                fixGates(2, 2 + inputSize + selectorBitSize, z);


                for (int y = 0; y < inputSize; y++) {
                    int *binArrRows = bin(y, selectorBitSize);

                    for (int k = 0; k < selectorBitSize; k++) {
                        if (binArrRows[k] == 0) {
                            selectorTruthValues[k] = !(data->truthValue[data->gates[z][2 + inputSize + k] - 10]);
                        } else {
                            selectorTruthValues[k] = (data->truthValue[data->gates[z][2 + inputSize + k] - 10]);
                        }
                    }

                    if (selectorBitSize == 1) {
                        valuesAfterAnd[y] = selectorTruthValues[0] && data->truthValue[data->gates[z][2 + y] - 10];
                    } else {
                        int temp1 = selectorTruthValues[0];

                        for (int i = 1; i < selectorBitSize; i++) {
                            temp1 = temp1 && selectorTruthValues[i];
                        }

                        valuesAfterAnd[y] = temp1 && data->truthValue[data->gates[z][2 + y] - 10];
                    }

                    free(binArrRows);
                }

                int temp2 = valuesAfterAnd[0];

                for (int j = 1; j < inputSize; j++) {
                    temp2 = temp2 || valuesAfterAnd[j];
                }

                data->truthValue[data->gates[z][2 + inputSize + selectorBitSize] - 10] = temp2;

                free(valuesAfterAnd);
                free(selectorTruthValues);
            }
        }

        printf("| ");

        for (int k = 0; k < data->outputSize; k++) {
            if (k + 1 != data->outputSize) {
                printf("%d ", data->truthValue[data->inputSize + k]);
            } else {
                printf("%d", data->truthValue[data->inputSize + k]);
            }
        }

        printf("\n");

        free(inputRows);
    }
}

void freeEverythingish() {
    for (int x = 0; x < data->allVariablesSize; x++) {
        free(data->allVariables[x]);
    }
    free(data->allVariables);

    for (int x = 0; x < data->gatesSize; x++) {
        free(data->gates[x]);
    }
    free(data->gates);
    free(data->input);
    free(data->output);
    free(data->truthValue);

    free(data);
}


int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "r");
    processFile(file);

    // sortTheCircuit(); I want to come back to this problem to see if I can fix it so it passes all test cases.
    printTruthTable();


    freeEverythingish();
}