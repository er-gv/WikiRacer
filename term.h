#ifndef SEARCH_TERM_H
#define SEARCH_TERM_H

typedef struct term_t{
    
        char* text;
        int depth;
        struct term_t* parent;
}Term;

int removeTerm(Term*);
void printTerm(const Term* t);
Term* makeTerm(const char*, int, const Term*);
int cmpTerms(const Term*, const Term*);
#endif