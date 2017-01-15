#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_codes.h"
#include "list.h"
#include "term.h"
#include "terms_queue.h"
#include "terms_dictionary.h"
#include "wiki_socket.h"
#include "wiki_parser.h"




List* findTermsChain(const char* beginTerm, const char* goalTerm, const unsigned maxSteps);
List* buildTermsChain(Term* goalTerm); //return linked list of terms from begin to goal


int main(int argc, char* argv[]){

    List* resultTermsChain = NULL;
    ListNode* node;
    const char* beginTerm = argv[1];
    const char* goalTerm = argv[2];
    const unsigned int maxSteps = atoi(argv[3]);
    if(argc != 4){
        fprintf(stderr, "Usage wiki_crawler <begin-term> <goal-term> <max-steps>.\n");
        return -1;
    }
    
    
    resultTermsChain = findTermsChain(beginTerm, goalTerm, maxSteps);
    if(resultTermsChain){
    //print the list
        printf("we got a path from \"%s\" to \"%s\". let\'s print it.\n", beginTerm, goalTerm);
        list_reset_iterator(resultTermsChain);
    
        while(node =list_get_next(resultTermsChain)){
            char* txt = (char*)node->data;
            puts(txt);
        }
        
    }
    else{
        printf("No path form \"%s\" to \"%s\" in length %u or shorter was found. :(.\n", beginTerm, goalTerm, maxSteps);
    }
    return 0;
}


List* findTermsChain(const char* beginTerm, const char* goalTerm, const unsigned maxSteps){
    
    int connectionStatus;
    char buff[2501];
    Term* currentTerm;
    Term* firstTerm = makeTerm(strdup(beginTerm), 0, NULL);
    List* TermsChain = NULL;
    buff[2500] = '\0';
    
    initTermsQueue();
    initDictionary(30);
    
    enqueueTerm(firstTerm);
    addTermToDictionary(firstTerm);    
    puts("entering the search queue loop...");
    printf("searching path from \"%s\" to \"%s\", %u steps max.\n", beginTerm, goalTerm, maxSteps);
    
    
    
    while(!isEmptyQueue()){
        
        currentTerm = (Term*)getFirst();
        dequeueTerm();
        //printf("currentTerm->text == %s.\n",currentTerm->text);
        if(strcmp(currentTerm->text,goalTerm)==0 && currentTerm->depth <= maxSteps){
            puts("Got Goal term!");
            TermsChain= buildTermsChain(currentTerm);
           
            break;
        }
        else if (currentTerm->depth < maxSteps){
            processWikiPage(currentTerm, goalTerm); //extract links from the page. add them to the dictionary and queue
            
            
            
            
            
        //readFragmentOfWikiPage(buff, 2500);
        //puts(buff);
            
        }
        
        //break;
    }
    printQueue();
    disposeQueue();//free the memory used for the search queue
    deleteDictionary();
    
    
    freeTCPResources();
    
    puts("exit findTermsChain");
    return TermsChain;
}

 List* buildTermsChain(Term* lastTerm){
     //return linked list of terms from begin to goal
     List* list = makeNewList((int (*)(void*))removeTerm);
     while(lastTerm!= NULL){
         
         list = addHead(list, strdup(lastTerm->text));
         lastTerm = lastTerm->parent;
    }
    return list;
     
} 