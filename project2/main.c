// shbang
// Project 2
// Judah Tanninen
// Description: Sorter for xp, two ways to sort, custom should be faster
// Usage: ./a.exe <sort_method> - sort_method will be either "standard" or "custom"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_LEN 6

// Print the array 
void printArr(int cnt, unsigned int **arr) {
    char *headers[] = {"SKILL_BREAKDANCING", "SKILL_APICULTURE", "SKILL_BASKET", "SKILL_XBASKET", "SKILL_SWORD", "TOTAL_XP"};
    for (int i=0; i < ARR_LEN; i++) {
        printf("%s\n", headers[i]);
        for (int j=0; j < cnt; j++) printf("%u\n", arr[i][j]);
        printf("\n");
    }
}

// Returns 0 if arguments are bad
int checkArgs(int argc, char **argv) {
    if (argc != 2) return 0;
    char *check = argv[1];
    if (strcmp(check, "standard") != 0 && strcmp(check, "custom")) return 0;
    return 1;
}

// Compare function for qsort
int compare(const void *a, const void *b) {
    return (*(unsigned int *)b - *(unsigned int *)a);
}

// Sorts all the arrays
void standard(int cnt, unsigned int **arr) {
    // Loop through array of arrays, and sort each one
    for (int i=0; i < ARR_LEN; i++) {
        qsort(arr[i], cnt, sizeof(unsigned int), compare);
    }
}


int main(int argc, char **argv) {
    // Check for valid arguments, pretty simple checks for this
    if (checkArgs(argc, argv) == 0) {
        printf("INVALID ARGS\n");
        printf("Usage: a.exe <sort_method>\n");
        return 1;
    }

    // Array of arrays
    unsigned int **arr = malloc(ARR_LEN * sizeof(unsigned int *));
    int capacity = 1; // Start at 1, multiply by ten, should divide nicely
    for (int i = 0; i < ARR_LEN; i++) arr[i] = malloc(capacity * sizeof(unsigned int)); // Initialize to size 1

    unsigned int tempArr[ARR_LEN];
    int cnt = 0;

    // Loop through stdin
    while (scanf("%u %u %u %u %u", &tempArr[0], &tempArr[1], &tempArr[2], &tempArr[3], &tempArr[4]) == 5) { // Loop until end
        // Check if we need to reallocate cuz we at the limit

        if (cnt == capacity) {
            // Increase the capacity by ten, should result in no over mallacing
            capacity *= 10;
            for (int i = 0; i < ARR_LEN; i++) arr[i] = realloc(arr[i], capacity * sizeof(unsigned int)); // Initialize to size 1
        }
        for (int i=0; i < ARR_LEN - 1; i++) {
            arr[i][cnt] = tempArr[i];
            arr[ARR_LEN - 1][cnt] += tempArr[i];
        }
        cnt++;
    }

    if (strcmp(argv[1], "standard") == 0) standard(cnt, arr);
    // if (strcmp(argv[1], "custom") == 0) custom(cnt, arr);

    printArr(cnt, arr);
}