#ifndef TERMS_QUEUE_H
#define TERMS_QUEUE_H

#include "list.h"
#include "term.h"




int initTermsQueue();
    
int enqueueTerm(Term* term);
int dequeueTerm();

Term* getFirst();
Term* getLast();

int isEmptyQueue();
int resetQueue();
int disposeQueue();
void printQueue();
#endif
