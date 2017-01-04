#include "terms_queue.h"
#include "error_codes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



static List* TermsQueue = NULL;



int initTermsQueue(){
    TermsQueue = makeNewList((int (*)(void*))removeTerm);
    return (TermsQueue != NULL? 0 : -1);
}


int enqueueTerm(Term* term){
    TermsQueue = addHead(TermsQueue, term);
    return (TermsQueue != NULL? 0 : -1);
    
}

int dequeueTerm(){
    //printQueue();
    TermsQueue = removeTail(TermsQueue, 0);
    return 0;
}

Term* getFirst(){
    return (Term*)getTail(TermsQueue);
}

Term* getLast(){
    return (Term*)getHead(TermsQueue);
}

int isEmptyQueue(){
    return isEmptyList(TermsQueue);
}

int resetQueue(){
    while(!isEmptyQueue()){
        dequeueTerm();
    }
    return 0;
}

int disposeQueue(){
    list_reset_iterator(TermsQueue);
    ListNode* node = list_get_next(TermsQueue);
    while(node){
        free(node);
        list_get_next(TermsQueue);
    }
    free(TermsQueue);
    TermsQueue = NULL;
    return 0;
}

void printQueue(){
    if(!TermsQueue){
        puts("The Queue is uninitilized.");
        return;
    }
        
    if(isEmptyQueue()){
        puts("The Queue is empty.");
        return;
    }
    list_reset_iterator(TermsQueue);
    ListNode* item;
    while((item = list_get_next(TermsQueue))!=NULL){
        Term* t = (Term*)item->data;
        printTerm(t);
    }
}


