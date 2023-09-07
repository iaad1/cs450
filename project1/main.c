// Project 1
// Judah Tanninen
// Description: Finds the determinant of a given matrix
// Must be of size n x n

// Includes
#include <stdio.h>

int main() {
    int n; // Matrix size.
    // Get the size of the arr.
    scanf("%d", &n);
    double arr[n][n];

    // Double for loop for getting matrix
    for (int i=0; i < n; i++) {
        for (int j=0; i < n; i++) {
            scanf("%lf", &arr[i][j]);
        }
    }

    

    return 0;
}