#include <stdio.h>
#include <stdlib.h>

struct Node {
	int data;
	struct Node* next;
};

struct Node* head = NULL;
struct Node* tail = NULL;
int listSize = 0;

void initializeList(int data) {
    head = (struct Node*) malloc(sizeof(struct Node)); // If you wove me, then set me fweeeee pwease!!! :D
    tail = NULL;
    head->data = data;
    listSize++;
    head->next = tail;
}

void insertNode(struct Node* dataNode) {
    if (tail == NULL) {
        if (head->data == dataNode->data) {
            free(dataNode);
            return;
        } else if (dataNode->data > head->data) {
            tail = dataNode;
            head->next = tail;
            listSize++;
        } else {
            tail = head;
            tail->next = NULL;
            head = dataNode;
            head->next = tail;
            listSize++;
        }

    } else { // Focus on this part now.
        struct Node* ptr = head;
        struct Node* ptr2 = head->next;

        while (ptr2 != NULL) {
            if (dataNode->data > ptr->data && dataNode->data < ptr2->data) {
                dataNode->next = ptr2;
                ptr->next = dataNode;
                listSize++;
                return;
            } else if (ptr2->data == dataNode->data || ptr->data == dataNode->data) {
                free(dataNode);
                return; // Hehe, we like easy tings.
            } else if (dataNode->data > ptr->data && ptr2->next == NULL) {
                tail->next = dataNode;
                tail = tail->next;
                listSize++;
                return;
            } else if (ptr->data > dataNode->data) { // Insert at the beginning because I'm not a cool boi. Does this comment make sense? No? Do I care? No.
                dataNode->next = head;
                head = dataNode;
                listSize++;
                return;
            }

            ptr = ptr->next;
            ptr2 = ptr2->next;
        }
    }
}

void deleteNode(int data) {
    if (head == NULL) {
        return;
    }

    struct Node* ptr = head;
    struct Node* ptr2 = head->next;

    if (data == ptr->data) {
        struct Node* temp = head;
        head = head->next;
        free(temp);
        listSize--;

        if (listSize == 1) {
            // This is because with the way my list works, if there is only one node, tail needs to be null because head and tail would be the same. With a bit of restructuring, this is an easy fix.
            // However, given that this isn't a super big deal in terms of this program, I am just making a note of it here. This isn't bad code given the circumstances though. It's just so that head and tail never point to the same node.
            tail = NULL;
        }
        return;
    }

    while (ptr2 != NULL) {
        if (ptr2->data == data) {
            if (ptr2->next != NULL) {
                ptr->next = ptr2->next;
                free(ptr2);
                listSize--;
                tail = ptr->next;
            } else {
                ptr->next = NULL;
                free(ptr2);
                listSize--;
                tail = ptr;
            }

            if (listSize == 1) {
                tail = NULL;
            }

            return;
        }
        ptr = ptr->next;
        ptr2 = ptr2->next;
    }
}

int isEmpty() {
    return head == NULL;
}

void printList() {
    struct Node* ptr = head;

    printf("%d :", listSize);

    while (ptr != NULL) {
        printf(" %d", ptr->data);
        ptr = ptr->next;
    }

    printf("\n");
}

void freeList(struct Node* headPtr) {
    struct Node* temp = headPtr;

    while (temp != NULL) {
        headPtr = headPtr->next;
        free(temp);
        temp = headPtr;
    }
}

int main() {
    char input;
    int data;
    struct Node* node;

    while (scanf(" %c%d", &input, &data) != EOF) { // Space to skip over whitespace so that doesn't cause any issues.
        if (input == 'i') {
            if (isEmpty()) {
                initializeList(data);
                printList();
            } else {
                node = (struct Node *) malloc(sizeof(struct Node));
                node->data = data;
                node->next = NULL;
                insertNode(node);
                printList();
            }
        } else if (input == 'd') {
            deleteNode(data);
            printList();
        }
    }

    if (!isEmpty()) {
        freeList(head);
    }
}
