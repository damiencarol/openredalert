#ifndef FIBHEAP_H
#define FIBHEAP_H

#include "video/Renderer.h"

class FibHeapEntry
{
public:
    friend class FibHeap;
    FibHeapEntry(void *value, Uint32 key);
    void *getValue()
    {
        return value;
    }
    void setKey(Uint32 k)
    {
        key = k;
    }
private:
    void insertInList(FibHeapEntry *value)
    {
        FibHeapEntry *tmp = value->left;

        value->left = this;
        tmp->right = right;

        right->left = tmp;
        right = value;
    }

    void link(FibHeapEntry *nparent)
    {
        /* Remove y from the rootlist */
        left->right = right;
        right->left = left;
        parent = nparent;

        /* Add y as x's child */
        if( nparent->child == NULL ) {
            nparent->child = this;
            left = this;
            right = this;
        } else {
            right = nparent->child->right;
            left = nparent->child;
            nparent->child->right->left = this;
            nparent->child->right = this;
        }

        /* Increase the degree of x */
        nparent->degree++;

        mark = 0;
    }

    void *value;
    FibHeapEntry *left;
    FibHeapEntry *right;
    FibHeapEntry *parent;
    FibHeapEntry *child;

Uint32 degree:
    31;
Uint32 mark:
    1;
    Uint32 key;
};

class FibHeap
{
public:
    FibHeap();
    ~FibHeap();
    FibHeapEntry *top()
    {
        return min;
    }
    FibHeapEntry *pop();
    void push(FibHeapEntry *value);
    void decreaseKey(FibHeapEntry *x, Uint32 k);
private:
    void consolidate();
    void cut(FibHeapEntry *x, FibHeapEntry *y);
    void cascading_cut(FibHeapEntry *x);
    FibHeapEntry *min;
    Uint32 n;
};

#endif
