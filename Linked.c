#include <stdlib.h>
#include <stdio.h>
#include "Linked.h"

// create new Node
struct Node *initialize(int *value, int movement, struct Node *child){
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    *(newNode->value) = *(value);
    *(newNode->value + 1) = *(value + 1);
    newNode->movement = movement;
    newNode->Child = child;
    return newNode;
}

// push to the front
struct Node *pushNode(struct Node *node, int *value, int movement){
    // create new node
    struct Node *newNode = initialize(value, movement, node);
    // change pointer
    node = newNode;
    //printf("\npushNode is called!\n");
    return node;
}

void printArray(struct Node *node){
    printf("[");
    while(node->Child != NULL){
        printf("(%d, %d), ", *(int *)(node->value), *(int *)(node->value + 1));
        node = node->Child;
    }
    printf("(%d, %d)]", *(int *)(node->value), *(int *)(node->value + 1));
    printf("]\n");
}