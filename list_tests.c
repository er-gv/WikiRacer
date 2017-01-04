
#include "list.h"
#include <stdio.h>
#include <stdlib.h>


void printList(List* list);
void testInitAndDelete();
void testAddHead();
void testAddTaill();
void testListSplit();
void testListNodesRemoval();
void printListState(const List* list);

int intDeleteFunc(int k){
    return 0;
}

int main(void){
    
    printf("Running list tests.\n");
    testInitAndDelete();
    testListNodesRemoval();
    testAddHead();
    testAddTail();
    testListSplit();
    return 0;
}

void testAddHead(){
    printf("\nTesting adding numbers 6 to 10 to head. expect to print 10 9 8 7 6\n");
    List* list = makeNewList(intDeleteFunc);
    printListState(list);
    list = addHead(list, 6);
    list = addHead(list, 7);
    list = addHead(list, 8);
    list = addHead(list, 9);
    list = addHead(list, 10);
    
    printListState(list);
    
    printList(list);
    
    list = deleteList(list);
}

void testAddTail(){
    printf("\nTesting adding numbers 6 to 10 to tail. expect to print  6 7 8 9 10\n");
    List* list = makeNewList(intDeleteFunc);
    printListState(list);
    list = addTail(list, 6);
    list = addTail(list, 7);
    list = addTail(list, 8);
    list = addTail(list, 9);
    list = addTail(list, 10);
    
    printListState(list);
    printList(list);
    
    list =deleteList(list);
    printListState(list);
    
    printList(list);
}

void testListNodesRemoval(){
    
    printf("\nTesting adding and removal\n");
    printf("first add 2 to head, 3 to tail and 1 to head. expect to print 1->2->3\n");
    List* list = makeNewList(intDeleteFunc);
    printListState(list);
    list = addTail(list, 2);
    list = addTail(list, 3);
    list = addHead(list, 1);
    printList(list);
    
    printf("now delete head. shouls show 2->3\n");
    list = removeHead(list, 1);
    printList(list);
    
    printf("now delete head. shouls show 3->null\n");
    list = removeHead(list, 1);
    printList(list);
    
    printf("now add 5 and -100 to tail and 13 to head. expect to seee 3->5-> -100\n");
    list = addTail(list, 5);
    list = addHead(list, 13);
    list = addTail(list, -100);
    printList(list);
    
    {
        printf("now find 3 and add 666 after it.\n");
        ListNode* node;
        list_reset_iterator(list);
        while((node = list_get_next(list)) && ((int)(node->data) != 3));
        listInsertAfter(list, node, (void*)666);
        printList(list);
        
        printf("now add -2 after 666.\n");
        listInsertAfter(list, node->next, -2);
        printList(list);
        
        
    }
    deleteList(list);
}


void testListSplit(){
    
    printf("\nTesting list split\n");
    printf("adding the first five products of five. expect to print 5->10->15->20-25\n");
    List* list = makeNewList(intDeleteFunc);
    List* suffix;
    printListState(list);
    {
        int k=25;
        while(k>0){
            addHead(list, k);
            k  = k-5;
        }
    }
    printList(list);
    
    {
        
        printf("now find 15 and split around it.\nprinting original list should show 5->10 and the new should be 20-25.\n");
        ListNode* node;
        list_reset_iterator(list);
        while((node = list_get_next(list)) && ((int)(node->data) != 15));
        suffix = listSplitAroundNode(list, node);
        printList(list);
        printList(suffix);
        free (node);
        
        printf("now Split 20->25 to 20->NULL and 25->NULL.\n");
        List* s2;
        list_reset_iterator(suffix);
        list_get_next(suffix);
        node = list_get_next(suffix);
        if(node)
            printf("first node of suffix content is %d\n",node->data);
        else
            printf("first node of suffix is NULL\n");
        s2 = listSplitAtNode(suffix, node);
        printListState(s2);
        printList(s2);
        printList(suffix);
        
        printf("Let\'s deplete both of them...\n");
        suffix = depleteList(suffix);
        s2 = depleteList(s2);
        printList(suffix);
        printList(s2);
        
        printf("And delete them.\n");
        suffix = deleteList(suffix);
        printf("suffix is "); printListState(suffix);
        s2 = deleteList(s2);
        printf("s2 is "); printListState(s2);
        
    }
    list = deleteList(list);
    printf("list is ");  printListState(list);
    
}


void testInitAndDelete(){
    printf("\nTesting creation of empty list and deleting it\n");
    List* list = makeNewList((int(*)(void*))NULL);
    if(list!= NULL){
        printf("Creating new list successfully\n");
        printListState(list);
        list = deleteList(list);
        if( list == NULL)
            printf("Deletion of empty list successfully\n");
        else
            printf("Deletion of empty list failed\n");
    }
    
}

void printListState(const List* list){
    
    int k =isEmptyList(list);
    switch(k){
        case 0:
            printf("Non empty list\n");
            break;
        case 1:
            printf("Empty list\n");
            break;
        case -1:
            printf("NULL list pointer\n");
            break;
    }
}

void printList(List* list){
    
    if(list){
        ListNode* node;
        list_reset_iterator(list);
        while(node = list_get_next(list))
            printf("%d -> ", (int)node->data);
        printf("NULL.\n");
    }
    else{
        printf("NULL list pointer.\n");
    }
    
    
}