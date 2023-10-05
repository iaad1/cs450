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
#define COUNT_SORT_LEN 10000 // Most should be less than 10k elements according to the assignment

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

// CUSTOM SORTING STUFF STARTS HERE

// Quick sort algo ripped off wikipedia https://en.wikipedia.org/wiki/Quicksort

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Returns an index, comparisons are for a DESC sort.
int partition(unsigned int *arr, int low, int high) {
    // get the pivot first by choosing the middle value
    int pindex = (high - low)/2 + low; // Pindexter
    unsigned int pivot = arr[pindex];

    // Get left and right index
    int i = low - 1; // Left
    int j = high + 1; // Right

    // Loop forever
    while(1) {
        do i++; 
        while(arr[i] > pivot);

        do j--;
        while(arr[j] < pivot);

        if (i >= j) return j;

        // Swap the left and right elements
        swap(&arr[i], &arr[j]);
    }

}

// My quick sort algo
void quickSort(unsigned int *arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p);
        quickSort(arr, p + 1, high);
    }
}



// Implements a count sort with quick sort on the side.
void csort(unsigned *arr, int cnt) {
    // Create a counting array, of size COUNT_SORT_LEN
    unsigned int *carr = calloc(COUNT_SORT_LEN, sizeof(unsigned int)); // Set to zeros using calloc
    // Create an array to quick sort, with values larger than 10k
    unsigned int *qarr = malloc( cnt * sizeof(unsigned int));
    

    // Couple variables for tracking places in carr and qarr
    int bad = 0; unsigned int val; // Bad is the number of numbers we've put into the quick sort array.
    for (int i=0; i <= cnt; i++) {
        val = arr[i]; // Current value
        if (val >= COUNT_SORT_LEN) { // check for bad case, add to bad array and increment bad
            // printf("bad: %d\n", bad);
            qarr[bad] = val;
            bad++;
            continue;
        }
        // printf("good: %d\n", val);
        // If we make it here, we are under 10k (IMPORTANT, actual count size is 9999, makes it simpler to handle 0s)
        carr[val] += 1; // Increment the count at this index (value), because we have zeros, we shouldn't need to subtract or add 1s anywhere
    }


    // We now have a count sort array, and an unsorted array for quick sort to sort
    // Sort the lame array with quicksort
    quickSort(qarr, 0, bad - 1);
    // Now that its quick sorted, put the two arrays back together, into the original array

    int i; // We wanna track where we at in the array.
    // First, since we are doing a descending sort, we start with the descending quick sort, and insert those into the array.
    for (i = 0; i <= bad; i++) { // Bad is the index left off on, which should be len - 1, hence the <=
        val = qarr[i];
        arr[i] = val;
    }

    // Now, loop backwards through the count sort, because it is sorted ascending
    for (int j = COUNT_SORT_LEN - 1; j >= 0; j--) { // Start at end, loop backwards
        val = carr[j];
        if (val == 0) continue; // Check if zero, if it is, skip.
        // If not, loop through and add j, val number of times
        for (int k=0; k < val; k++) {
            arr[i] = j;
            i++;
        }
    }


}

// CUSTOM SORTING ENDS HERE

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

void custom(int cnt, unsigned int **arr) {
    for (int i=0; i < ARR_LEN; i++) {
        csort(arr[i], cnt);
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
    int cnt = 1;

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

    // printf("Count: %d\n", cnt);

    if (strcmp(argv[1], "standard") == 0) standard(cnt, arr);
    if (strcmp(argv[1], "custom") == 0) custom(cnt, arr);

    printArr(cnt, arr);
}