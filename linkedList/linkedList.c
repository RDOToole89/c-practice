#include <stdio.h>
#include <stdlib.h>


// Define the Node structure
// Purpose: Represents a node in the linked list
// Members:
//   - data: The data stored in the node
//   - next: A pointer to the next node in the list
//-------------------------------
typedef struct Node {
  int data;
  stuct Node *next;
} Node;

//-------------------------------
// Function: createNode
// Purpose: Creates a new node with the given data
// Input: The data to be stored in the node
// Output: A pointer to the newly created node
//-------------------------------
Node* createNode(int data) {
  // Allocate memory for the new node
  Node* newNode = (Node*)malloc(sizeof(Node));

  if (newNode == NULL) {
    fprintf(stderr, "Failed to allocate memory for new node\n");
    exit(EXIT_FAILURE);
  }

  // Initialize the node's data and set the next pointer to NULL
  newNode->data = data;
  newNode->next = NULL;

  return newNode;
}
