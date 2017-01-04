#include <stdio.h>
#include "term.h"
#include "terms_dictionary.h"

int main(int argc, char* argv[]){
    
    printf("\n\nRunning B-Tree tests.\n");
    
    initDictionary(atoi(argv[1]));
    for(int i='a';i<argv[2][0]; i++)      {
        char buff[3];
        sprintf(buff, "%c%c", i,i);
        addTerm(makeTerm(buff, i-'a'+2, NULL));
    }
    printTermsLevelOrder();
    
    
    printf("Delete tree memory...\n");
    deleteDictionary();
    return 0;
}
