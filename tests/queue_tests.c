#include <stdio.h>
#include "terms_queue.h"


int main(void){
    
    char* list[] ={"Irish","The","beat","never","You"};
    Term* prev = NULL;
    
    printf("\nRunning Queue tests.\n\n");
    printQueue();
    initTermsQueue();
    printQueue();
    for(int i=0; i<5; i++){
        Term* t = makeTerm(list[i], (i+1)*3, (const Term*)prev);
        prev = t;
        enqueueTerm(t);
    }
    if(isEmptyQueue()){
        printf("The new queue is empty.\n");
    }
    else {
        printf("The new queue is not empty. Here is the content:\n");
        printQueue();
    }
    puts("we dequqe 3 items and print content after each one");
    for(int i=0; i<3; i++){
        dequeueTerm();
        printQueue();
    }
    {
        Term* t = makeTerm("56", 56,(const Term*)0x56);
        enqueueTerm(t);
    }
    {
        Term* t = makeTerm("7 wonders", 7, (const Term*)0x7);
        enqueueTerm(t);
    }
    dequeueTerm();
    {
        Term* t = makeTerm("never", -6, (const Term*)0x666);
        enqueueTerm(t);
    }
    {
        Term* t = makeTerm("8 marvels", -86, (const Term*)0x386);
        enqueueTerm(t);
    }
    
    printQueue();
    disposeQueue();
    printQueue();
    return 0;
}