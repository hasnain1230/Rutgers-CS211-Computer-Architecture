#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define malloc(X) calloc(X, 1)


/*
 * Things I Need:
 * Multiply function (can yoink it from mexp and just repurpose it)
 * CreateIdentityMatrix Function
 * Transpose Function
 */


double** matrixX;
double** matrixY;
size_t matrixXRowSize;
size_t matrixXColSize;

double** make2DArray(size_t rowSize, size_t colSize) { // Makes 2D array based on sizes.
    double **arr = (double **) calloc(rowSize, sizeof(double *)); // For my purposes, I cannot use malloc here because of the way memory is allocated using malloc. Thus, using calloc fixes my problems when calling "multiplyTogether."
    // double **arr = (double **) malloc(rowSize * sizeof(double *));
    if (arr == NULL) { // If calloc failed. 
        puts("Failed");
        exit(EXIT_FAILURE);
    }

    for (int x = 0; x < rowSize; x++) {
        arr[x] = (double *) calloc(colSize, sizeof(double)); // Create 2D array, again with calloc.
        // arr[x] = (double *) malloc(colSize * sizeof(double));

        if (arr[x] == NULL) {
            puts("Failed");
            exit(EXIT_FAILURE);
        }
    }

    return arr;
}

double** multiplyTogether(double** matrix1, double** matrix2, size_t m1R, size_t m1C, size_t m2C) { // I really feel like there is a better way to solve this. I may revisit this problem some other time. I'd be super interested in a better solution...
    // Here, the two matrices to multiply in are passed. After that, it is the same thing as mexp.
    double **solution = make2DArray(m1R, m2C);
    for (int x = 0; x < m1R; x++) {
        for (int y = 0; y < m2C; y++) {
            for (int z = 0; z < m1C; z++) {
                solution[x][y] += (matrix1[x][z] * matrix2[z][y]);
            }
        }
    }

    return solution;
}

void traverse(double** arr, size_t rowSize, size_t colSize) { // Prints array / matrix.
    for (int x = 0; x < rowSize; x++) {
        for (int y = 0; y < colSize; y++) {
            printf("%.0lf\n", arr[x][y]); // Cuts of the decimal point and prints value as-is.
        }
    }
}

void makeIdentityMatrix(double** matrix, size_t size) { // Takes given matrix and makes it an identity function. In this program, since we know that `matrix` will always be a square matrix, rowSize and colSize do not need to be worried about.
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

void applyEroToRow(double** matrix1, double** matrix2, int rowIndex, int operationIndex, size_t size, double factor, char mode) { // Either does the multiply or divide function to the matrix. This is based on the invert matrices algorithim.
    if (mode == 'd' || operationIndex == -1) { // Divide by factor.
        for (int x = 0; x < size; x++) {
            matrix1[rowIndex][x] /= factor;
            matrix2[rowIndex][x] /= factor;
        }
    } else if (mode == 'm' && 1) { // Don't need the && 1. Just an IDE trick. // Here, we simply multiply by the factor and subtract it from the original matrix.
        for (int x = 0; x < size; x++) {
            matrix1[operationIndex][x] -= (matrix1[rowIndex][x] * factor);
            matrix2[operationIndex][x] -= (matrix2[rowIndex][x] * factor);
        }
    }
}


double** invertMatrix(double** matrix1, size_t size) { // Inverts a given matrix assuming it is invertible and always a square matrix.
    double **matrix2 = make2DArray(size, size);
    makeIdentityMatrix(matrix2, size);
    double factor;

    for (int x = 0; x < size; x++) {
        factor = matrix1[x][x];
        applyEroToRow(matrix1, matrix2, x, -1, size, factor, 'd');

        for (int y = x + 1; y < size; y++) {
            factor = matrix1[y][x];
            applyEroToRow(matrix1, matrix2, x, y, size, factor, 'm');
        }
    }

    for (int x = size - 1; x >= 0; x--) {
        for (int y = x - 1; y >= 0; y--) {
            factor = matrix1[y][x];
            applyEroToRow(matrix1, matrix2, x, y, size, factor, 'm');
        }
    }

    return matrix2;
}

double** transposeMatrix(double** matrix, size_t rowSize, size_t colSize) { // Transposes the matrix. This was a littly tricky to figure out because the indexes can get confusing, but it is not that bad. Pretty simple to understand.
    double **transposedMatrix = make2DArray(colSize, rowSize);

    for (int x = 0; x < rowSize; x++) {
        for (int y = 0; y < colSize; y++) {
            transposedMatrix[y][x] = matrix[x][y]; // Simply just swap the indexes. Go row, then column, rather than col then row. 
        }
    }

    return transposedMatrix;
}

void freeArray(double** arr, size_t rowSize) { // Frees a given array + size.
    for (int x = 0; x < rowSize; x++) {
        free(arr[x]);
    }

    free(arr);
}

void processFile(FILE* file) { // Processes either the training or data file.
    char fileName[6]; // Temporary buffer to store file name. This is to determine if the file is the training or data file because they are both to be processed in a certain way. 
    fscanf(file, "%5s", fileName);

    if (fgetc(file) != 10) { // This tests to see if the first line of the file was what we were expecting (either data, or train). If we do not read a newline character (ASCII value 10), then something has gone wrong and we need to terminate.
        printf("Invalid input for file: %s", fileName);
        exit(1);
    }

    fscanf(file, "%zd", &matrixXColSize); // Could use strtol here instead of scanf, but that just creates new problems for a simple program. Also, Professor wants scanf even though it is discouraged. 
    matrixXColSize++;
    fscanf(file, "%zd", &matrixXRowSize);

    matrixX = make2DArray(matrixXRowSize, matrixXColSize); // Corresponding matrices from the assignment.
    matrixY = make2DArray(matrixXRowSize, 1);


    for (int x = 0; x < matrixXRowSize; x++) {
        for (int y = 0; y < matrixXColSize; y++) {
            if (y == 0) {
                matrixX[x][y] = 1.0; // First column must be all 1s.
            } else {
                fscanf(file, "%lf", &matrixX[x][y]);
            }
        }

        if (strcmp(fileName, "train") == 0) { // If we are reading training data, send it to matrixY because the last column in that file is the final prices for the houses. Thus, it goes in matrixY. 
            fscanf(file, "%lf", &matrixY[x][0]);
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *train = fopen(argv[1], "r");
    processFile(train);

    // Lines 170 - 175: These lines compute the bulk of the algorithm. They find the weights for each house based on the attributes. This is the main part of the program.
    double** transposeX = transposeMatrix(matrixX, matrixXRowSize, matrixXColSize);
    double** multipliedMatrix = multiplyTogether(transposeX, matrixX, matrixXColSize, matrixXRowSize, matrixXColSize);
    double** inverseMatrix = invertMatrix(multipliedMatrix, matrixXColSize);
    freeArray(multipliedMatrix, matrixXColSize);
    multipliedMatrix = multiplyTogether(inverseMatrix, transposeX, matrixXColSize, matrixXColSize, matrixXRowSize);
    double** weights = multiplyTogether(multipliedMatrix, matrixY, matrixXColSize, matrixXRowSize, 1);

    // We need to free basically everything here so that all the variables above can be free to use later should we need to. This reduces the total amount of memory used by this program.
    freeArray(transposeX, matrixXColSize);
    freeArray(multipliedMatrix, matrixXColSize);
    freeArray(inverseMatrix, matrixXColSize);
    freeArray(matrixX, matrixXRowSize);
    freeArray(matrixY, matrixXRowSize);
    // Don't forget to free weights. 
    // Weights is not freed yet because we still need it to compute the final prices.

    size_t xColSize = matrixXColSize;
    
    FILE* data = fopen(argv[2], "r");
    processFile(data); // Free matrixX and matrixY

    size_t xPrimeColSize = matrixXColSize;

    if (xColSize != xPrimeColSize) { // Make sure we can actually multiply the two matrices. 
        puts("Attributes quantity from train and data are not the same!");
        exit(1);
    }

    multipliedMatrix = multiplyTogether(matrixX, weights, matrixXRowSize, matrixXColSize, 1);

    traverse(multipliedMatrix, matrixXRowSize, 1); // Print final prices.

    freeArray(weights, matrixXColSize); // Here, we free everything that we used again (after freeing from before). We needed to free before, and again now, because if we did not, then we would have unallocated memory because we new space again within the functions which would cause a memory leak if we did not allocate.
    freeArray(matrixX, matrixXRowSize);
    freeArray(matrixY, matrixXRowSize);

    return 0;
}
