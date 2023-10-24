// shbang
// Judah Tanninen
// CS450
// Description: Two tree implementations of a griefer list
// Usage ./a.out <type> <file_name>
// Types implemented: scapegoat, avl


// Includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Struct definitions
struct node { // Node should work with both avl and scapegoat, for ease of use
    char key[16]; // Should be 16 digits
    int cnt; // Number of times banned. Unique
    int *sids; // Stupid lame array of server ids
    int timestamp; // Holds the most recent ban timestamp
    int height; // Only for avl
    struct node *left;
    struct node *right;
    struct node *parent; // Only for scapegoat.
};
typedef struct node *Node; // So we don't have to use "struct node *" all the time.

// Function declarations
bool validArgs(int, char **);
int nodeHeight(Node);
int nodeBalance(Node);
Node rotateRight(Node);
Node rotateLeft(Node);
Node avlInsert(Node, char *, int, int);
Node avl(FILE *);
Node treeSearch(Node, char[16]);

// LAME AH array function
int *append(int *, int *, int ); // Stupid annoying banned from server twice shit
// Why grant, why??

int max(int, int);

// Main program
int main(int argc, char **argv) {
    // Do the clock stuff (don't worry about it till the end)
    clock_t t = clock();

    // Begin file stuff, open the file located in argv[2]
    FILE *fp;
    fp = fopen(argv[2], "r+"); // Read only
    if (fp == NULL) {
        perror("File Error: ");
        return 1;
    }
    // Create a node pointer that the below functions will then assign
    Node root = NULL;

    // Check if we are doing avl or scapegoat
    if (strcmp(argv[1], "avl") == 0) {
        root = avl(fp);
    } else if (strcmp(argv[1], "scapegoat")) {
        // root = scapegoat(fp);
    }
    fclose(fp); // Close the file, dont need. (maybe get rid of this for speed?)
    // printf("key: %s, cnt: %d, timestamp: %d\n", root->key, root->cnt, root->timestamp);

    char in[16];
    // Tree should now be created, take stdin in and find the matching node.
    while(scanf("%s", in) == 1) {
        // Make a search
        Node res = treeSearch(root, in);
        if (res == NULL) {
            printf("%s is not currently banned from any servers\n");
            continue;
        }
        // Found it, print the relevant information
        printf("%s was banned from %d servers. most recently on: %d\n", res->key, res->cnt, res->timestamp);
    }




    t = clock() - t;
    int time_taken = (((double)t)/CLOCKS_PER_SEC) * 1000000; // Microseconds
    printf("total time in microseconds: %d\n", time_taken); 
}


// Makes sure the arguments are valid, to avoid seg faults
bool validArgs(int argc, char **argv) {
    if (argc != 3) return false;
    // Check that arg 1 is either "scapegoat" or "avl"
    if (strcmp(argv[1], "scapegoat") != 0 && strcmp(argv[1], "avl") != 0) return false;
    return true;
}


/////////////////
// AVL FUNCTIONS
/////////////////

// Takes a file of griefers, and adds them all to an avl tree. Then, returns the root of the tree.
Node avl(FILE *fp) {
    Node root = NULL; // Create a node root
    char line[32]; // Assume a max line length of 32, should handle it fine
    char name[16];
    int serverId;
    int timestamp;

    // Loop through each line, inserting as we go
    while(fgets(line, 33, fp) != NULL) {
        sscanf(line, "%s %d %d", name, &serverId, &timestamp); // Pull the data.
        root = avlInsert(root, name, serverId, timestamp);
    }
    return root; // Return the root.
}

// Insert function, does a lot of stuff, including rotating
Node avlInsert(Node root, char *key, int sid, int timestamp) {
    // if root is null, create a new node and return it
    if (root == NULL) {
        Node new = malloc(sizeof(struct node)); // New node
        new->left = new->right = NULL; // Set legs to null
        strcpy(new->key, key);
        new->cnt = 1;
        new->sids = malloc(1 * sizeof(int));
        new->sids[0] = sid;
        new->timestamp = timestamp;
        new->height = 1; // Default height to 1.
        return new;
    }

    // Comparison
    int cmp = strcmp(key, root->key);
    if (cmp < 0) { // 
         root->left = avlInsert(root->left, key, sid, timestamp);
    } else if (cmp > 0) {
        root->right = avlInsert(root->right, key, sid, timestamp);
    } else {
        root->sids = append(root->sids, &(root->cnt), sid); // Update the server ids
        if (timestamp > root->timestamp) root->timestamp = timestamp; // Update the latest timestamp
    }

    // Update the height of the current node
    root->height = 1 + max(nodeHeight(root->left), nodeHeight(root->right));

    // Get the balance
    int bal = nodeBalance(root);

    // Now, we do the rotating, based on the balance

    // First, left heavy, if balance is greater than 1
    if (bal > 1) {
        cmp = strcmp(key, root->left->key);
        if (cmp < 0) return rotateRight(root); // If key less than the roots left key
        // Else, double rotate
        root->left = rotateLeft(root->left);
        return rotateRight(root); // Could simplify this a bit
    }
    // Second, right heavy, balance left than -1
    if (bal < -1) {
        cmp = strcmp(key, root->right->key);
        if (cmp > 0) return rotateLeft(root); // If key greater than the roots right key
        // Else, double rotate
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// Rotate left
Node rotateLeft(Node x) {
    Node y = x->right;
    Node T2 = y->left;

    // Perform the rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = 1 + max(nodeHeight(x->left), nodeHeight(x->right));
    y->height = 1 + max(nodeHeight(y->left), nodeHeight(y->right));

    return y; // New root after rotation
}

// Rotate right
Node rotateRight(Node y) {
    Node x = y->left;
    Node T2 = x->right;

    // Perform the rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = 1 + max(nodeHeight(y->left), nodeHeight(y->right));
    x->height = 1 + max(nodeHeight(x->left), nodeHeight(x->right));

    return x; // New root after rotation
}

// Recursive tree search function that gets the node matching the given key.
// If not found, NULL is returned
Node treeSearch(Node root, char key[16]) { // Key will always be around 16 chars
    if (root == NULL) return NULL;
    int cmp = strcmp(key, root->key); // compare the two strings
    if (cmp > 0) { // Key is greater than root->key, go right
        return treeSearch(root->right, key);
    }
    if (cmp < 0) { // Key is less that root->key, go left
        return treeSearch(root->left, key);
    }
    // Keys match, return root.
    return root;
}


// Stupid lame append to array function, won't do anything if server id is in array already.
int *append(int *arr, int *size, int el) {
    int len = (*size);
    // Loop through arr, if el is in it, return the original array.
    for (int i=0; i<len; i++) {
        if (el == arr[i]) return arr;
    }
    // Not in arr, realloc for new arr
    int *newArr = (int *)realloc(arr, (len + 1) * sizeof(int));
    // Add element to the end
    newArr[len] = el;
    // Increment the size pointer
    (*size) += 1;
    return newArr;
}

// Returns the max of two given numbers, used so i don't have to put ternarys everywhere
int max(int a, int b) {
    return a > b ? a : b;
}

int nodeHeight(Node root) {
    if (root == NULL) return 0;
    return root->height;
}

// Returns the balance of the tree, left - right
int nodeBalance(Node root) {
    if (root == NULL) return 0; // Base case if root is nothing. shouldn't happen?

    int lh = (root->left != NULL) ? root->left->height : 0;
    int rh = (root->right != NULL) ? root->right->height : 0;

    // Return the balance
    return lh - rh;
}