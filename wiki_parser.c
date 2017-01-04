//
// Created by ergv on 20/11/16.
//

#include "wiki_parser.h"
#include "wiki_socket.h"

#include "terms_dictionary.h"
#include "terms_queue.h"
#include "error_codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define FAIL    -1
//#define response_BUFFER_SIZE 4096*512
#define HTML_FRAGMENT_LEN 4096


static int TERM_BUFFER_SIZE =100;

typedef enum e_parse_result{GOT_TERM, GOT_INVALID_TERM, GOT_PREFIX, BUFFER_TOO_SMALL, NO_MORE_TERMS_IN_FRAGMENT} ParseResult;

int parseFragment(const char* fragment, const Term* current_term, const char* goal_term, unsigned int* suffixLen, unsigned int* termsFound );
ParseResult readNextTerm(char* stream, char* newTermBuffer, char** suffix);


int processWikiPage(const Term* current_term, const char* goal_term){
    
    char response[HTML_FRAGMENT_LEN+1];
    //unsigned int charsToRead = HTML_FRAGMENT_LEN;
    unsigned int reminderChars = 0;
    
    unsigned int termsFound = 0;
    int status = 0; 
    int nFragments =0;
    int found = 0;
    
    
    //int found = 0;
    //if returns negative the SSL_send fail and we abort
    
    puts("@processWikiPage");
    printf("looking for %s. It's queue order is %d.\n", current_term->text, current_term->depth);
    fetchWikiPage(current_term->text);// bascically do http request from wiki
    if(status < 0)
        return status;
    response[HTML_FRAGMENT_LEN]='\0';
    //Read response which is HTML document. We want to extract out of it
    //all links to adjusent internal wiki pages.
    //memset(response, 0, (HTML_FRAGMENT_LEN) *sizeof(char));
    do{
        //read new fragment of html. the responce buffer may contains a prefix content from the previous fragment
        if(reminderChars>0){
            unsigned prefix =HTML_FRAGMENT_LEN -reminderChars;
            //printf("reset response buffer. new it contains %s.\n", response);
            strncpy(response, response+prefix*sizeof(char), reminderChars);
            memset(response+reminderChars*sizeof(char), 0, prefix *sizeof(char));
            //printf("and after moving the last %d chars to the front of the buffer it holds %s.\n", reminderChars, response);
        }
        else{
            memset(response, 0, HTML_FRAGMENT_LEN *sizeof(char));
        }
        //puts("read a fragment of wiki page");
        
        status = readFragmentOfWikiPage(response+reminderChars*sizeof(char), HTML_FRAGMENT_LEN-reminderChars);
        //puts("done reading.");
        if(status <0){
            fprintf(stderr, "failed to recive() http.\n");
            return status;
        }
    
        nFragments++;
        //fprintf(stdout, "processing chunck #%d.\n"/*\'s content is %s...\n"*/, nFragments/*, response*/);
      
        //found=parseFragment(response, current_term, goal_term, &reminderChars, &termsFound);
        
    }while(status> 0 && !found);
    
    printf("Total terms found in page: %u.\n\n", termsFound);
    puts("~processWikiPage");
    return status;
}

int parseFragment(const char* fragment, const Term* current_term, const char* goal_term, unsigned int* suffixLen, unsigned int* termsFound ){
    
    char* buffer = malloc(sizeof(char)*(TERM_BUFFER_SIZE+1));

    unsigned int done=0;
    char* offset  =fragment;
    char* newBufffer;
    int found = 0;
    *suffixLen = 0;
    ParseResult parseResult; //
    Term* newTerm; 
    
    //suffix = readNextTerm(suffix, buffer);
    while(!done && !found){
        
       // printf("looking for term inside %s\n", offset);
       
        buffer[TERM_BUFFER_SIZE]='\0';//put a marker at the end of the buffer. if we fill up the buffer it will be overiden.
        parseResult =readNextTerm(offset, buffer, &offset);
        
        switch(parseResult){
            
            case GOT_TERM:
                buffer[0] = toupper(buffer[0]);
                newTerm = makeTerm(buffer, current_term->depth+1, current_term);
                if(!isExists(newTerm)){
                    addTermToDictionary(newTerm);
                    
                    if(found = !strcmp(goal_term, buffer)){
                        //Since we found the goal term there is no need to scan and BFT the rest of the terms in the queue
                        //and wait untill the goal term reach the other end of the queue
                        found = 1;
                        puts("got goal term!");
                        resetQueue();
                    }
                    enqueueTerm(newTerm);//, q.top.depth, q.top);
                    //puts("print queue after adding new term");
                    //printQueue();
                }
                else{
                    removeTerm(newTerm);
                }
                    (*termsFound)++;
                break;
            case GOT_INVALID_TERM:
                //puts("got invalid term. continue reading the reminder of the html fragment");
                break;
            case BUFFER_TOO_SMALL:
                
                //puts("The buffer for parsing a term is too small. Let/'s double it and try to parse the term again");
                newBufffer = realloc(buffer, sizeof(char)*(TERM_BUFFER_SIZE*2+1));
                if(newBufffer){
                    buffer = newBufffer;
                    TERM_BUFFER_SIZE*=2;
                }
                else{
                    done = 1;
                    puts("Failed to realloc term buffer");
                }
                
                break;
            case GOT_PREFIX:
                //printf("Got prefix of a term while reached end of fragment. need to copy this suffix to the front of fragment buffer before reading the next one.\n strlen(buffer) == %u.\n", strlen(buffer));
                *suffixLen = strlen(buffer);
                done =1;
                break;
            case NO_MORE_TERMS_IN_FRAGMENT:
                //puts("No more terms in this html fragment. No need to copy stuff to the front of the fragment buffer before loading it with new html content");
                done = 1;
                break;
            default:
                puts("@parseFragment: something got wrong!");
                done = 1;
                break;
        }
        
        if((offset-fragment)/sizeof(char)>=HTML_FRAGMENT_LEN){
            done = 1;
        }
        /*
        if(status)
            puts(buffer);
                puts("Make new term...");
                Term* newTerm = makeTerm(buffer, current_term->depth+1, current_term);
              
                if(!isExists(newTerm)){
                    addTermToDictionary(newTerm);
                    
                    
                    *found = !strcmp(goal_term, buffer);
                    if(*found){
                        //Since we found the goal term there is no need to scan and BFT the rest of the terms in the queue
                        //and wait untill the goal term reach the other end of the queue
                        puts("got goal term!");
                        resetQueue();
                    }
                    enqueueTerm(newTerm);//, q.top.depth, q.top);
                    puts("print queue after adding new term");
                    printQueue();
                }
                (*termsFound)++;
                
            }
        }
        if(totalCharsRead)else{
            
        }
       
        printf("read %d, total %d.\n", nCharsRead, totalCharsRead);
        if(buffer[0]!='\0'){
            if((nCharsRead<TERM_BUFFER_SIZE && buffer[nCharsRead]=='\0') || buffer[TERM_BUFFER_SIZE] == '\0'){
                puts(buffer);
                puts("Make new term...");
                Term* newTerm = makeTerm(buffer, current_term->depth+1, current_term);
              
                if(!isExists(newTerm)){
                    addTermToDictionary(newTerm);
                    
                    
                    *found = !strcmp(goal_term, buffer);
                    if(*found){
                        //Since we found the goal term there is no need to scan and BFT the rest of the terms in the queue
                        //and wait untill the goal term reach the other end of the queue
                        puts("got goal term!");
                        resetQueue();
                    }
                    enqueueTerm(newTerm);//, q.top.depth, q.top);
                    puts("print queue after adding new term");
                    printQueue();
                }
                (*termsFound)++;
                
            }
            else{
                puts("break out of the while loop and return to processWikiPage");
                //break;//there is the case where we copyied only the prefix of a token.
                
            }
            fprintf(stdout, "%u chars remain to process at current fragment\n", HTML_FRAGMENT_LEN-totalCharsRead);
        }
        */
    }//while
    //printf("At the end of parseFragment, *suffixLen=%u.\n", *suffixLen);
    //puts("~parseFragment");
    //fprintf(stdout, "List of terms we got so far:\n");
    //printTermsLevelOrder();
    free(buffer);
    return found;
}


ParseResult readNextTerm(char* stream, char* newTermBuffer, char** suffix){
    
    char pattern[] = "<a href=\"/wiki/";
    int i=0;
    char* ptr;
    //read the stream of chars untill end or finding new wiki href.
    //copy the href to newTermBuffer
    newTermBuffer[0]='\0';
    ptr = strstr(stream, pattern);
    if(!ptr){
        ptr = strstr(stream, "<");
        if(!ptr)
            return NO_MORE_TERMS_IN_FRAGMENT; //no tokens in stream    
        else{
            (*suffix) = ptr;
            //puts("got \'<' without the reminder wiki link prefix. more at the next fragment");
            strcpy(newTermBuffer, "<");
            return GOT_PREFIX;
        }
        
    }
    ptr = strstr(stream, pattern);
   // printf("got something, prefix: %s\n", ptr);
    ptr+=strlen(pattern);
    while(ptr[i] && i<TERM_BUFFER_SIZE && !isspace(ptr[i]) && ptr[i] !='\"' && ptr[i]!=':' && ptr[i]!='#'&& ptr[i]!='<'){
        
        newTermBuffer[i] =ptr[i];
        //puts(newTermBuffer);
        i++;
    }
    *suffix = &ptr[i];
   // printf("%u, last char we got: %c. newTermBuffer == \"%s\"\n", (ptr-stream)/sizeof(char), ptr[i], newTermBuffer);//nCharsRead 
    if(ptr[i] == '\"'){
        //got a token. there is room for it n the buffer. we managed to copy it fully.
        newTermBuffer[i]='\0';
        printf("We got term %s\n", newTermBuffer);
        if(strcmp(newTermBuffer, "Main_Page")==0 || strstr(newTermBuffer,"_(disambiguation)")!=NULL){
            //bed token. We don't want the main page or a link to disambiguation page.
            //mark to the caller that we have not found a term at this time.
            newTermBuffer[0] = '\0';
            return GOT_INVALID_TERM;
        }
        return GOT_TERM;
    }
    else if(ptr[i]=='\0'){ //The term's suffix is in the next html fragment.
        strcpy(newTermBuffer, ptr-strlen(pattern)-sizeof(char)*i);
        return GOT_PREFIX;
    }
    else if(i==TERM_BUFFER_SIZE ){
        *suffix -= i*sizeof(char);
        return BUFFER_TOO_SMALL;
    }
    return GOT_INVALID_TERM;
}


