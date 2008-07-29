// fibheap.cpp
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#include "include/fibheap.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "SDL/SDL_types.h"


FibHeapEntry::FibHeapEntry(void *value, Uint32 key)
{
    this->value = value;
    this->mark = 0;
    this->key = key;
    this->degree = 0;

    left = this;
    right = this;
    child = NULL;
    parent = NULL;
}

FibHeap::FibHeap()
{
    min = NULL;
    n = 0;
}

FibHeap::~FibHeap()
{
    //   if( min != NULL )
    //     delete min;
}

FibHeapEntry *FibHeap::pop()
{

    FibHeapEntry *z, *child, *tmp;
    z = min;
    if( z == NULL )
        return NULL;

    child = z->child;
    /* insert z's children in the min-list */
    if( child != NULL ) {
        tmp = child;
        /* Set all parents to NULL (since they will be in rootlist) */
        do {
            tmp->parent = NULL;
            tmp = tmp->right;
        } while( tmp != child );

        /* Add the list to the rootlist */
        z->insertInList(child);
    }

    z->left->right = z->right;
    z->right->left = z->left;

    if( z == z->right )
        min = NULL;
    else {
        min = z->right;
        consolidate();
    }

    n--;
    return z;
}

void FibHeap::push(FibHeapEntry *value)
{
    /* insert x in the rootlist if there is one */
    if( min != NULL ) {
        min->insertInList(value);
    }

    if( min == NULL || value->key < min->key )
        min = value;
    n++;
}

void FibHeap::decreaseKey(FibHeapEntry *x, Uint32 k)
{
    FibHeapEntry *y;

    if( k > x->key )
        return;
    x->key = k;
    y = x->parent;

    if( y != NULL && x->key < y->key ) {
        cut(x, y);
        cascading_cut( y );
    }

    if( x->key < min->key )
        min = x;

}

void FibHeap::consolidate()
{
    Uint32 i, last, d;
    FibHeapEntry *x, *w, *y, *tmp;
    FibHeapEntry **A;

    A = new FibHeapEntry* [n];

    /* Set all entries in A to NULL */
    memset( A, 0, sizeof(FibHeapEntry*)*n );
    //   for( i = 0; i < n; i++ )
    //     A[i] = NULL;

    /* Start with the first value */
    w = min;

    /* Add that value at the correct place */
    //x = w;
    //d = x->degree;
    //A[d] = x;


    last = 0;
    tmp = min->right;
    while( tmp != min ) {
        last++;
        tmp = tmp->right;
    }

    i = 0;
    /* Take the next value */
    while( i <= last ) {
        //      w = w->right;
        x = w;
        d = x->degree;

        w = w->right;

        while( A[d] != NULL && d < n ) {
            y = A[d];

            if( x->key > y->key ) {
                tmp = x;
                x = y;
                y = tmp;
            }

            // last--;
            if( y == w )
                w = w->right;

            y->link(x);

            A[d] = NULL;
            d++;

        }

        A[d] = x;
        i++;
    }

    min = NULL;

    for( i = 0; i < n; i++ ) {
        if( A[i] != NULL ) {
            A[i]->left = A[i];
            A[i]->right = A[i];
            A[i]->parent = NULL;

            /* concat A[i] with the rootlist in H if there is one */
            if( min != NULL ) {
                A[i]->right = min->right;
                min->right = A[i];
                A[i]->left = min;
                A[i]->right->left = A[i];
            }

            if( min == NULL || A[i]->key < min->key )
                min = A[i];
        }
    }

    delete A;
}

void FibHeap::cut( FibHeapEntry *x, FibHeapEntry *y )
{
    y->degree--;
    if( x->right == x )
        y->child = NULL;
    else {
        if( y->child == x )
            y->child = x->right;
        x->left->right = x->right;
        x->right->left = x->left;
    }
    x->parent = NULL;

    x->right = min->right;
    x->left = min;
    min->right = x;
    x->right->left = x;

    x->mark = 0;
}

void FibHeap::cascading_cut( FibHeapEntry *y )
{
    FibHeapEntry *z;

    z = y->parent;
    if( z != NULL ) {
        if( y->mark == 0 )
            y->mark = 1;
        else {
            cut( y, z );
            cascading_cut( z );
        }
    }
}
