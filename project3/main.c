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

// Defines
#define ALPHA 0.5

// Struct definitions
struct node { // Node should work with both avl and scapegoat, for ease of use
    char key[16]; // Should be 16 digits
    int cnt; // Number of times banned. Unique
    int *sids; // Stupid lame array of server ids
    int timestamp; // Holds the most recent ban timestamp
    int height; // Only for avl
    int size; // For scapegoat.
    struct node *left;
    struct node *right;
    struct node *parent; // Only for scapegoat.
};
typedef struct node *Node; // So we don't have to use "struct node *" all the time.

// Helper function declarations

bool validArgs(int, char **);
// LAME AH array function
int *append(int *, int *, int ); // Stupid annoying banned from server twice shit
// Why grant, why??
int max(int, int);
Node treeSearch(Node, char[16]);
Node *appendNode(Node *, int, Node);

// AVL tree declarations
int nodeHeight(Node);
int nodeBalance(Node);
Node rotateRight(Node);
Node rotateLeft(Node);
Node avlInsert(Node, char *, int, int);
Node avl(FILE *);

// Scapegoat tree declarations
int nodeSize(Node);
Node scapegoat(FILE *);
Node scapeInsert(Node, Node *, char *, int, int, int *, int);
Node rebuild(Node);
void ioTraverse(Node, Node *, int *);
Node buildFromArr(Node *, int, int);


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
    } else if (strcmp(argv[1], "scapegoat") == 0) {
        root = scapegoat(fp);
    }
    fclose(fp); // Close the file, dont need. (maybe get rid of this for speed?)
    // printf("key: %s, cnt: %d, timestamp: %d\n", root->key, root->cnt, root->timestamp);

    char in[16];
    // Tree should now be created, take stdin in and find the matching node.
    while(scanf("%s", in) == 1) {
        // Make a search
        Node res = treeSearch(root, in);
        if (res == NULL) {
            printf("%s is not currently banned from any servers\n", in);
            continue;
        }
        // Found it, print the relevant information
        printf("%s was banned from %d servers. most recently on: %d\n", res->key, res->cnt, res->timestamp);
    }

    // Done with all the shtuff, finish the clock stuff.
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

///////////////////////
// SCAPEGOAT FUNCTIONS
///////////////////////

// Takes a file to griefers, and adds them all to a scapegoat tree. Then, returns the root.
Node scapegoat(FILE *fp) {
    Node root = NULL; // Create a node root pointer
    Node bad = NULL; // Create a pointer that we can check for when we encounter a bad insert.
    char line[32]; // Assume a max line length of 32, should handle it fine
    char name[16];
    int serverId;
    int timestamp;
    int nodes = 0;
    // Loop through each line, inserting as we go
    while(fgets(line, 33, fp) != NULL) {
        sscanf(line, "%s %d %d", name, &serverId, &timestamp); // Pull the data.
        root = scapeInsert(root, &bad, name, serverId, timestamp, &nodes, 0);
        if (bad != NULL) root = rebuild(bad);
        bad = NULL;
    }
    return root; // Return the root.
}

Node scapeInsert(Node root, Node *bad, char key[16], int sid, int timestamp, int *nodes, int depth) {
    int ncnt = (*nodes); // Number of nodes inserted so far.
    int allowed = ncnt > 0 ? (log(ncnt)/log(1/ALPHA)) + 1 : 1; // Number that we can insert down to before requiring a rebalance.
    // printf("nodes: %d, depth: %d, allowed: %d\n", ncnt, depth, allowed);
    // if root is null, create a new node and return it
    if (root == NULL) {
        root = malloc(sizeof(struct node)); // New node
        root->left = root->right = NULL; // Set legs to null
        strcpy(root->key, key);
        root->cnt = 1;
        root->sids = malloc(1 * sizeof(int));
        root->sids[0] = sid;
        root->timestamp = timestamp;
        root->parent = NULL;
        root->size = 0; // Default size to 0, will get incremented a little later
        (*nodes) += 1;
    }

    if (depth > allowed) {
        (*bad) = root; // Assign the bad node. We will trigger a rebuild from here.
    }
    // Comparison
    int cmp = strcmp(key, root->key);
    if (cmp < 0) {
         root->left = scapeInsert(root->left, bad, key, sid, timestamp, nodes, depth + 1);
         root->left->parent = root; // Set the parent
    } else if (cmp > 0) {
        root->right = scapeInsert(root->right, bad, key, sid, timestamp, nodes, depth + 1);
        root->right->parent = root; // Set the parent
    } else {
        root->sids = append(root->sids, &(root->cnt), sid); // Update the server ids
        if (timestamp > root->timestamp) root->timestamp = timestamp; // Update the latest timestamp
    }

    // Update the size of the root.
    root->size = 1 + nodeSize(root->left) + nodeSize(root->right);
    return root;
}


// Function for rebalancing the tree, returns the root of the tree.
Node rebuild(Node bad) {
    // Begin traversing up the list until we find the highest node that is bad
    Node root = bad;

    // Get the root
    while(root->parent != NULL) {
        root = root->parent;
    }

    // Create an array of the length of the size of node
    int len = root->size; int index = 0;
    Node *arr = malloc(len * sizeof(Node));
    ioTraverse(root, arr, &index);
    // Arr should now be an array in order.
    Node new = buildFromArr(arr, 0, len - 1);
    new->parent = NULL;
    return new;
}

// Build subtree from given array, which should be in order
Node buildFromArr(Node *arr, int low, int high) {
    if (low > high) return NULL; // Base case
    int middle = (low + high) / 2;// Find the middle point.
    Node root = arr[middle];
    root->left = buildFromArr(arr, low, middle - 1);
    root->right = buildFromArr(arr, middle + 1, high);
    root->size = 1 + nodeSize(root->left) + nodeSize(root->right);
    return root;
}

void ioTraverse(Node root, Node *arr, int *index) {
    if (root == NULL) return;
    ioTraverse(root->left, arr, index);
    arr[(*index)++] = root;
    ioTraverse(root->right, arr, index);
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
        return rotateRight(root); // Could simplify this a bit, but its not worth it for my brain.
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
    Node y = x->right; // the right node
    Node z = y->left; // the left of that node

    y->left = x; // Do the rotates
    x->right = z;

    // Update the new heights of the moved nodes.
    x->height = 1 + max(nodeHeight(x->left), nodeHeight(x->right));
    y->height = 1 + max(nodeHeight(y->left), nodeHeight(y->right));

    return y; // The new root returned
}

// Rotate right, same as left, but swapped.
Node rotateRight(Node y) {
    Node x = y->left;
    Node z = x->right;

    x->right = y;
    y->left = z;

    y->height = 1 + max(nodeHeight(y->left), nodeHeight(y->right));
    x->height = 1 + max(nodeHeight(x->left), nodeHeight(x->right));

    return x;
}

int nodeHeight(Node root) {
    if (root == NULL) return 0;
    return root->height;
}

int nodeSize(Node root) {
    if (root == NULL) return 0;
    return root->size;
}

// Returns the balance of the tree, left - right
int nodeBalance(Node root) {
    if (root == NULL) return 0; // Base case if root is nothing. shouldn't happen?

    int lh = (root->left != NULL) ? root->left->height : 0;
    int rh = (root->right != NULL) ? root->right->height : 0;

    // Return the balance
    return lh - rh;
}

////////////////////
// HELPER FUNCTIONS
////////////////////

// Functions related to the trees, but not directly related to avl trees.


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

// Doesn't increment for you.
Node *appendNode(Node *arr, int len, Node el) {
    Node *newArr = realloc(arr, (len + 1) * sizeof(struct node));
    newArr[len] = el;
    return newArr;
}

// Returns the max of two given numbers, used so i don't have to put ternarys everywhere
int max(int a, int b) {
    return a > b ? a : b;
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