// Project 1
// Judah Tanninen
// Description: Finds the determinant of a given matrix
// Must be of size n x n

// Includes
#include <stdio.h>
#include <stdlib.h>

int main() {
    int n; // Matrix size.
    // Get the size of the arr.
    scanf("%d", &n);
    double **arr = malloc(n * sizeof(double *));

    // Double for loop for getting matrix
    for (int i=0; i < n; i++) {
        arr[i] = malloc(n * sizeof(double));
        for (int j=0; j < n; j++) {
            scanf("%lf", &arr[i][j]); // Scan the double into the array.
        }
    }
    double determinant = 1; // Start the determinant at positive one.
    double *temp; // Temp pointer for row swaps.

    // Loop through the columns, beginning at the first
    for (int i=0; i < n; i++) {
        // Normalize the pivot position
        for (int j=i; j < n; j++) { // Start at the pivot
            double el = arr[j][i];
            if (el == 0) continue; // Skip if zero, need a value
            if (i == j) continue; // Skip if pivot row and has value, useless swap.
            // // Swap this column so theres a value at the pivot.
            temp = arr[j];
            arr[j] = arr[i];
            arr[i] = temp;
            determinant *= -1; // Row swapping, determinant sign flips.
            break;
        }
    }
    // Should be normalized now

    // Using forward elimination, make all elements below the pivot elements zero.
    for (int i=0; i < n; i++) {
        double pivot = arr[i][i]; // Pivot element
        determinant *= pivot; // Becuase we are eliminating forward, we can just grab the value here.
        for (int j=i+1; j < n; j++) { // Below is forward elimination.
            double *row = arr[j];
            double el = row[i];
            double mult = el / pivot;
            for (int k=i; k < n; k++) {
                arr[j][k] -= mult * arr[i][k]; // Subtract the row by a scalar of another row. (may actually be columns)
            }
        }
    }

    printf("%lf\n", determinant); // Print the output.
    return 0;
}