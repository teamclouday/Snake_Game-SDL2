#ifndef LINKED_H
#define LINKED_H

// linked list
struct Node{
    int value[2];
    int movement;
    struct Node *Child;
};

struct Node *pushNode(struct Node *node, int *value, int movement);
struct Node *initialize(int *value, int movement, struct Node *child);
void printArray(struct Node *node);

#endif