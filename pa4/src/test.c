#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void decToBinary(int n)
{
    // array to store binary number
    int binaryNum[32];

    // counter for binary array
    int i = 0;
    while (n > 0) {

        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--)
        printf("%d", binaryNum[j]);
}

// Driver program to test above function
int main()
{
    for (int x = 0; x < pow(2, 20); x++) {
        decToBinary(x);
        printf("\n");
    }

    return 0;
}
