#pragma once
#include <sal.h>

struct LinkedList
{
    struct LinkedList *next;
    int data;
};

typedef struct LinkedList LinkedList;

_Ret_maybenull_ LinkedList *AllocateNode();