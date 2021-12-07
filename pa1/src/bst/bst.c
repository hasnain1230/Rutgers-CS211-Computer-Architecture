#include <stdio.h>
#include <stdlib.h>

struct Node { // Why??? I want to explore why this happens later. Why does order matter in a struct?
    int data;
    struct Node* left;
    struct Node* right;
};

struct Node* root = NULL;

void initializeTree(struct Node* node) { // I am sure it is possible to get insert to create root node too, but for readablity, I'll leave it like this for now.
    root = (struct Node*) malloc(sizeof(struct Node));
    root->data = node->data;
    root->left = NULL;
    root->right = NULL;
}


struct Node* insert(struct Node* rootPtr, struct Node* node) {
    if (rootPtr == NULL) {
        rootPtr = (struct Node*) malloc(sizeof(struct Node));
        rootPtr->data = node->data;
        rootPtr->left = NULL;
        rootPtr->right = NULL;
    }

    if (rootPtr->data > node->data) {
        rootPtr->left = insert(rootPtr->left, node);
    } else if (rootPtr->data < node->data) {
        rootPtr->right = insert(rootPtr->right, node);
    }

    return rootPtr;
}

void printInOrder(struct Node* rootPtr) {
    if (rootPtr == NULL) {
        return;
    } else {
        printf("(");
        printInOrder(rootPtr->left); // Traverse left subtree.
        printf("%d", rootPtr->data);

        printInOrder(rootPtr->right); // Traverse right subtree.
        printf(")");
        return;
    }
}

int search(struct Node* rootPtr, int key) {
    if (rootPtr == NULL) {
        return 0;
    }
    if (rootPtr->data == key) {
        return 1;
    }
    if (rootPtr->data > key) {
        return search(rootPtr->left, key);
    } else {
        return search(rootPtr->right, key);
    }
}

struct Node* deleteNode(struct Node* rootPtr, int key) { // I think I could also do this with a return type of void. I'd be interested to try. I think doing so would involve more malloc
    // which is something that I simply find too tiring and difficult to do.
    if (rootPtr == NULL) { // Base case
        return rootPtr;
    }

    if (rootPtr->data > key) { // Traverse left subtree just like print function. This is to find where the key even is.
        rootPtr->left = deleteNode(rootPtr->left, key);
    } else if (rootPtr->data < key) { // Traverse right subtree just like print function. This is to find where the key even is.
        rootPtr->right = deleteNode(rootPtr->right, key);
    } else { // If we found the key. Now, we restructure the tree.
        if (rootPtr->left == NULL) {
            struct Node* rightLeaf = rootPtr->right;

            if (rootPtr->left == NULL || rootPtr->right == NULL) {
                free(rootPtr);
            }

            return rightLeaf;
        } else if (rootPtr->right == NULL) {
            struct Node* leftLeaf = rootPtr->left;

            if (rootPtr->right == NULL || rootPtr->left == NULL) {
                free(rootPtr);
            }

            return leftLeaf;
        }

        struct Node* smallestNodeInTree = rootPtr->right;

        while (smallestNodeInTree != NULL && smallestNodeInTree->left != NULL) { // Find smallest value in left subtree.
            smallestNodeInTree = smallestNodeInTree->left;
        }

        rootPtr->data = smallestNodeInTree->data;
        rootPtr->right = deleteNode(rootPtr->right, smallestNodeInTree->data);
    }

    return rootPtr;
}

void freeTree(struct Node* rootPtr) {
    if (rootPtr == NULL) {
        free(rootPtr);
        return;
    }

    freeTree(rootPtr->left);
    freeTree(rootPtr->right);

    free(rootPtr);
}

int main() {
    char input;
    int data;
    int count = 0;
    // Count can be handled in a better way for future reference.
    struct Node* node = (struct Node *) malloc(sizeof(struct Node));;

    while (scanf(" %c%d", &input, &data) != EOF) { // Space to skip over whitespace so that doesn't cause any issues.
        if (input == 'i') {
            node->data = data;
            node->left = NULL;
            node->right = NULL;

            if (count == 0) {
                initializeTree(node);
                printf("inserted\n");
                count++;
            } else {
                if (search(root, data) == 1) {
                    printf("not inserted\n");
                } else {
                    insert(root, node);
                    printf("inserted\n");
                    count++;
                }
            }
        } else if (input == 's') {
            if (search(root, data)) {
                printf("present\n");
            } else {
                printf("absent\n");
            }
        } else if (input == 'p') {
            // printf("\n");
            printInOrder(root);
            printf("\n");
        } else if (input == 'd') {
            if (search(root, data) == 0) {
                printf("absent\n");
            } else {
                printf("deleted\n");
                root = deleteNode(root, data);
                count--;
            }
        }
    }

    free(node);
    freeTree(root);

    // Testing purposes only. To be delete later.

    struct Node* node1 = (struct Node*) malloc(sizeof(struct Node));
    node1->data = 2;
    node1->left = NULL;
    node1->right = NULL;

    struct Node* node2 = (struct Node*) malloc(sizeof(struct Node));
    node2->data = 1;
    node2->left = NULL;
    node2->right = NULL;

    struct Node* node3 = (struct Node*) malloc(sizeof(struct Node));
    node3->data = 5;
    node3->left = NULL;
    node3->right = NULL;

    struct Node* node4 = (struct Node*) malloc(sizeof(struct Node));
    node4->data = 3;
    node4->left = NULL;
    node4->right = NULL;

    struct Node* node5 = (struct Node*) malloc(sizeof(struct Node));
    node5->data = 6;
    node5->left = NULL;
    node5->right = NULL;

    struct Node* node6 = (struct Node*) malloc(sizeof(struct Node));
    node6->data = 4;
    node6->left = NULL;
    node6->right = NULL;

    free(node1);
    free(node2);
    free(node3);
    free(node4);
    free(node5);
    free(node6);

    return 0;
}
