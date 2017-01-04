#include "term.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int removeTerm(Term* term){
    if(term){
        #ifdef DEBUG_MODE
            puts("There is a term object to remove");
            printTerm(term);
        #endif
        free((void*)term->text);
        free(term);
        #ifdef DEBUG_MODE
            puts("... And now it\'s gone!");
        #endif
    }
    return 0;
}

void printTerm(const Term* t){
    printf("{Text:   \"%s\",\tcount:  %d,\tparent:  %p}\n", t->text, t->depth, t->parent);
}


Term* makeTerm(const char* text, int depth, const Term* parent){
    Term* term = malloc(sizeof(Term));
    term->text = strdup(text);
    term->depth = depth;
    term->parent = parent;
    return term;
}

int cmpTerms(const Term* t1, const Term* t2){
    return strcmp(t1->text, t2->text);
}