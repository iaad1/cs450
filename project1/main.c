// Project 1
// Judah Tanninen
// Description: Finds the determinant of a given matrix
// Must be of size n x n

// Includes
#include <stdio.h>
#include <stdlib.h>


void printArr(double **arr, int n) {
    for (int i=0; i < n; i++) {
        for (int j=0; j < n; j++)
            printf("%lf ", arr[i][j]);
        printf("\n");
    }
    
}

int main() {
    int n; // Matrix size.
    // Get the size of the arr.
    scanf("%d", &n);
    double **arr = malloc(n * sizeof(double *));

    // Double for loop for getting matrix
    for (int i=0; i < n; i++) {
        arr[i] = malloc(n * sizeof(double));
        for (int j=0; j < n; j++) {
            scanf("%lf", &arr[i][j]);
        }
    }
    printf("test\n");
    int swaps;
    double *temp; // Temp pointer for row swaps.

    // Loop through the columns, beginning at the first
    for (int i=0; i < n; i++) {
        // Normalize the pivot position
        for (int j=i; j < n; j++) { // Start at the pivot
            double el = arr[i][j];
            if (el == 0) continue; // Skip if zero, need a value
            // Swap this column so theres a value at the pivot.
            temp = arr[j];
            arr[j] = arr[i];
            arr[i] = temp;
        }
    }
    printArr(arr, n);


    return 0;
}