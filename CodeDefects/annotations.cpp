#include "annotations.h"
#include <malloc.h>

_Ret_maybenull_ LinkedList *AllocateNode()
{
    LinkedList *result = static_cast<LinkedList *>(malloc(sizeof(LinkedList)));
    return result;
}

_Ret_maybenull_ LinkedList *AddTail(_Maybenull_ LinkedList *node, int value)
{
    if(node == NULL)
        return NULL;
    // finds the last node
    while(node->next != nullptr)
    {
        node = node->next;
    }

    // appends the new node
    LinkedList *newNode = AllocateNode();
    if(newNode != NULL)
    {
        newNode->data = value;
        newNode->next = 0;
        node->next = newNode;
    }

    return newNode;
}