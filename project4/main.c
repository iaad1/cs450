// shbang
// Judah Tanninen
// November 8th, 2023
// Project 4
// Napsack: auto loot.

// Includes
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Defines
#define NAME_LEN 128 // Length of name, is actually 127?
#define ITEM_MAX 128 // Maxinum number of items
#define CAP_MAX 1200 // Max capacity weight.

// Struct definitions
struct item {
    char name[NAME_LEN]; // Could be 127, but its fine
    int value;
    int weight;
};

// Custom struct for use with dynamic programming
struct maxValue {
    int maxValue;
    int maxWeight;
    bool saved; // Set to false, will be true if the value has been saved and can be used by the dynamic function.
    bool items[ITEM_MAX]; // Max of 128 items, will be a 1 to signify what item is being used, matches position in "arr" in the main function
};

// Array of saved values, this is a global so the dynamic function can access it.
struct maxValue savedValues[CAP_MAX];

// Function definitions
int parseInput(struct item *);
struct maxValue getMaxVal(struct item *, int, int);


// Main
int main() {
    int cap; // Capacity
    scanf("%d", &cap); // Get the capacity from the first line.
    // Create an array, 128 items long, may not use all of them.
    struct item arr[ITEM_MAX];
    int len = parseInput(arr); // Returns the number of items in an array.

    // Loop through all the savedValues and set their saved bool to false
    for (int i=0; i < CAP_MAX; i++) savedValues[i].saved = false;
    // Get the best possible value
    struct maxValue bestVal = getMaxVal(arr, cap, len);

    // Now, we're done, just print all the info

    // Loop through all the items
    for (int i=0; i < len; i++) {
        // If item is used, print its info
        if (bestVal.items[i] == true) {
            printf("%s, %d, %d\n", arr[i].name, arr[i].weight, arr[i].value);
        }
    }
    // Print the max value and weight
    printf("final weight: %d\n", bestVal.maxWeight);
    printf("final value: %d\n", bestVal.maxValue);

    return 0;
}

// Reads standard in an returns a count of how many rows it read.
int parseInput(struct item * arr) {
    int val; int weight; char name[NAME_LEN]; int cnt = 0;
    // While loop for reading input
    while (scanf(" %[^;];%d;%d", name, &weight, &val) == 3) {
        arr[cnt].value = val;
        arr[cnt].weight = weight;
        strcpy(arr[cnt].name, name);
        cnt++;
    }
    return cnt;
}

struct maxValue getMaxVal(struct item *arr, int cap, int len) {
    struct maxValue returnVal, checkVal; // Create a struct to return, and one for recursive checking
    // Initial setting all items to false.
    for (int i=0; i < len; i++) returnVal.items[i] = false; // Initialize all the item usages to false.
    // Set the max values and weights to zero
    returnVal.maxValue = returnVal.maxWeight = 0;

    // Base case, if capacity is zero, return an empty maxValue
    if (cap == 0) return returnVal;

    // Another base case, if we have a saved value for this capacity (dynamic programming baby) return that.
    if (savedValues[cap].saved == true) return savedValues[cap];

    // Create a temp item, for ease of looking at while looping.
    struct item tempVal;

    // Loop over all the items.
    for (int i=0; i < len; i++) {
        tempVal = arr[i];
        // Check a couple things before we continue looping.

        if (tempVal.weight > cap) continue; // If weight is greater then capacity, it don't fit :(
        if (returnVal.items[i] == true) continue; // If we have used this item, skip to the next.
        
        checkVal = getMaxVal(arr, (cap - tempVal.weight), len);
        // Check to see if we want to use this check value, (value is greater)
        if ((tempVal.value + checkVal.maxValue) > returnVal.maxValue && checkVal.items[i] == false) {
            returnVal = checkVal;
            returnVal.maxValue += tempVal.value; // Set new max value
            returnVal.maxWeight += tempVal.weight; // Add the weight too.
            returnVal.items[i] = true; // Set this item to used.
        }
    }
    returnVal.saved = true;
    // Now, set the saved value so we can use it again!
    savedValues[cap] = returnVal;
    return returnVal;
}
