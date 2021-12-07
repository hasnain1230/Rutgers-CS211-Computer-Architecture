#include <stdio.h>
#include <stdlib.h>

size_t size;
int power;

int** processFile(FILE *file) {
    fscanf(file, "%zd", &size);
    int** arr = (int **) malloc((size * size) * sizeof(int *));

    for (int x = 0; x < size; x++) {
        arr[x] = (int *) malloc(size * sizeof(int));

        for (int y = 0; y < size; y++) {
            fscanf(file, "%d", &arr[x][y]);
        }
    }

    fscanf(file, "%d", &power);
    fclose(file);

    return arr;
}

int** createDuplicateArray(int** originalArr) {
    int** arr = (int **) malloc(size * size * sizeof(int *));

    for (int x = 0; x < size; x++) {
        arr[x] = (int *) malloc(size * sizeof(int));

        for (int y = 0; y < size; y++) {
            arr[x][y] = originalArr[x][y];
        }
    }

    return arr;
}

void multiplyTogether(int** originalArr, int** prevSolution, int** solutionArr) { // I really feel like there is a better way to solve this. I may revisit this problem some other time. I'd be super interested in a better solution...
    int product = 0;
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            for (int z = 0; z < size; z++) {
                product += (originalArr[x][z] * prevSolution[z][y]);
            }
            solutionArr[x][y] = product;
            product = 0;
        }
    }
}

void copyArr(int** previousSolution, int** solution) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            previousSolution[x][y] = solution[x][y];
        }
    }
}


void printSolutionArr(int** arr) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (y + 1 == size) { // For the record, me having to do this is really dumb. The auto grader should account for trailing white spaces at the end that make no difference in actuality.
                printf("%d", arr[x][y]);
            } else {
                printf("%d ", arr[x][y]);
            }
        }

        printf("\n");
    }
}


void makeIdentityMatrix(int** matrix) { // Refactoring this code could make this more efficient. That is, having less functions that do more to decrease the amount of loops in other functions. It works for now though as a V1.
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (x == y) {
                matrix[x][y] = 1;
            } else {
                matrix[x][y] = 0;
            }
        }
    }
}

void freeArr(int** arr) {
    for (int x = 0; x < size; x++) {
        free(arr[x]);
    }

    free(arr);
}

void matrixToNPower(int** original, int** solution) {
    int** previousSolution = createDuplicateArray(original);

    for (int x = 1; x < power; x++) {
        multiplyTogether(original, previousSolution, solution);
        copyArr(previousSolution, solution);
    }

    freeArr(previousSolution);
}

int main(int argc, char* argv[]) {
    FILE *file = fopen(argv[1], "r");

    int **original = processFile(file);

    if (power > 0) {
        int **solution = createDuplicateArray(original);
        
        matrixToNPower(original, solution);
        printSolutionArr(solution);

        freeArr(original);
        freeArr(solution);

        return 0;

    } else if (power == 0) {
        makeIdentityMatrix(original);
        printSolutionArr(original);

        freeArr(original);

        return 0;
    } else {
        freeArr(original);
        perror("The power you want to take the matrix to is not a positive number!!");
        return 1;
    }
}