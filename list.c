#include "list.h"
#include "error_codes.h"
#include <stdlib.h>
#include <stdio.h>




struct list_t {
    ListNode* head;
    ListNode* tail;
    ListNode* itr;
    int (*deleteObjFunc)(void*) ;
    //int (*hasNext)();
    //void* (*getNext)();
};


ListNode* makeNewNode(void* content){
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    node->data =content;
    node->next = NULL;
    return node;
}

List* makeNewList( int(*deleteObjFunc)(void*)){
    List* list = (List*)malloc(sizeof(List));
    list->head = list->tail = NULL;
    list->itr = NULL;
    list->deleteObjFunc = deleteObjFunc;
    
    return list;
}


List* addHead(List* list, const void* data){
    
    ListNode* newNode = makeNewNode(data);
    if(!newNode)
        return NULL;
    newNode->next = list->head;
    list->head = newNode;
    if(NULL ==list->tail){
        list->tail = newNode;
        list->itr = NULL;
    }
    return list;
}

List* addTail(List* list, const void* data){
    
    ListNode* newNode = makeNewNode(data);
    if(!newNode)
        return NULL;
    
    if(NULL ==list->tail){
        //the list is empty and the new node is both head and tail
        list->head = newNode;   
        list->itr = NULL;
    }
    else 
        //add the new node right after the tail
        list->tail->next = newNode;
    //update tail pointer
    list->tail = newNode;    
    return list;
}

List* listInsertBefore(List* list, ListNode* beforeMe, const void* addition){
    if(beforeMe== NULL || isEmptyList(list))
        return addHead(list, addition);
    else{
        //add a new node after beforeMe
        ListNode* newNode = makeNewNode(addition);
        ListNode* next = beforeMe->next;
        newNode->next = next;
        beforeMe->next = newNode;
        //put the data from beforeMe into the newNode so now the addition data cold be placed in the beforeMe node.
        newNode->data = beforeMe->data;
        beforeMe->data = addition;
        
        if(list->tail == beforeMe)
            list->tail = newNode;
        return list;
    }
}

List* listInsertAfter(List* list, ListNode* afterMe, const void* addition){
    if(afterMe== NULL || isEmptyList(list))
        return addTail(list, addition);
    else{
        ListNode* newNode = makeNewNode(addition);
        ListNode* next = afterMe->next;
        newNode->next = next;
        afterMe->next = newNode;
        if(list->tail == afterMe)
            list->tail = newNode;
        return list;
    }
}

void* getHead(List* list){
    if(list->head)
        return list->head->data;
    return NULL;
}


void* getTail(List* list){
    if(list->tail)
        return list->tail->data;
    return NULL;
}

List* removeHead(List* list, int deleteData){
    
    if(NULL ==list->head){
        return NULL;// it's an empty list
    }
    if(list->head == list->tail){
        //the list has a single node;
        if(deleteData){
            //acctually remove the data stored in this node
            list->deleteObjFunc(list->head->data);
        }
        free(list->head);
        list->head = list->tail = NULL;
        list->itr = NULL;
    }
    else{
        ListNode* newHead = list->head->next;
        if(list->itr == list->head){
            list->itr = newHead;
        }
        if(deleteData){
            //acctually remove the data stored in this node
            list->deleteObjFunc(list->head->data);
        }
        free(list->head);
        list->head = newHead;
    }
    return list;
    
}

List* removeTail(List* list, int deleteData){
    
    if(NULL ==list->tail){
        return NULL;// it's an empty list
    }
    if(list->head == list->tail){
        //the list has a single node;
        
        if(deleteData){
            //acctually remove the data stored in this node
            
            list->deleteObjFunc(list->tail->data);
        }
        
        free(list->tail);
        
        list->head = list->tail = NULL;
        list->itr = NULL;
    }
    else{
        if(list->itr == list->tail){
            list->itr = NULL;
        }
        ListNode* current = list->head;
        ListNode* prev;
        while(current != list->tail){
            prev = current;
            current = current->next;
        }
        list->tail = prev;
        prev->next = NULL;
        if(deleteData){
            //acctually remove the data stored in this node
            list->deleteObjFunc(current->data);
        }
        free(current);
        
    }
    
    return list;
    
}


List* listSplitAroundNode(List* original, ListNode* pivot){
    //what if original list is empty or has only one or two nodes?
    //Also, who ever calls this should hold a pointer to pivot->data, otherwise we will lose
    //access to pivot->data once we free(pivot) at the end of this function.
 
    //find the node before pivot. set original tail there
    List* result = makeNewList(original->deleteObjFunc);
    ListNode* current = original->head;
    if(pivot== original->head || pivot== original->tail){
        return NULL;
        //no, throw exception    
    }
    //iterate toward for pivot...
    while(current && current->next!= pivot )
        current = current->next;
    
    //set the head for the new list that begins after the pivot
    result->head = pivot->next;
    
    //setting tail for the former part of the original list
    original->tail = current;
    original->tail->next = NULL;
    
    //now iterate on the suffix of the original list, pointed by reasult->head.
    //find it's last node and make it the the tail for the new list, result
    current = result->head;
    while(current->next )
        current =current->next;
    result->tail = current;
    
    //free(pivot);
    return result;    
}

List* listSplitAtNode(List* original, ListNode* pivot){
    //split this list to prefix and suffix list.
    //prefix goes from head to one before pivot. suffix goes from pivot to original's list tail.
    
    //find the node before pivot. move theset original tail there
    List* result = makeNewList(original->deleteObjFunc);
    ListNode* current = original->head;
    if(original->head == original->tail ||pivot == original->head){
        return NULL;
    }
    //iterate toward for pivot...
    while(current && current->next!= pivot )
        current = current->next;
    
    //set the head for the new list that begins after the pivot
    result->head = pivot;
    
    //setting tail for the former part of the original list
    original->tail = current;
    original->tail->next = NULL;
    
    //now iterate on the suffix of the original list, pointed by reasult->head.
    //find it's last node and make it the the tail for the new list, result
    current = result->head;
    while(current->next )
        current =current->next;
    result->tail = current;
    
    //free(pivot);
    return result;
    
    
}


List* depleteList(List *list){
    
    ListNode* current = list->head;
    while(current != list->tail){
        ListNode* next = current->next;
        list->deleteObjFunc(current->data);
        free(current);
        current = next;
    }
    list->deleteObjFunc(list->tail->data);
    free(list->tail);
    list->head = list->tail = list->itr = NULL;
    return list;
}

int isEmptyList(const List* list){
    if(list)
        return list->head == NULL? 1:0;
    return -1;
}


unsigned listSize(List* list){
    
    unsigned count =0;
    for(ListNode* ptr = list->head;ptr; ptr = ptr->next, count++);
    return count;
}


List* deleteList(List *list){
    if(list && !isEmptyList(list)){
        depleteList(list);
        free(list);
    }
    return NULL;

}

void list_reset_iterator(List* list){
     list->itr = NULL;  
}

int list_has_next(const List *list){
    //if current is NULL then it stands before the head. otherwise it is somewere between head and tail 
    //and we want to test if it's on the tail node.
    if(isEmptyList(list))
      return 0;
    return (list->itr !=NULL ? (list->itr != list->tail):1);  
}

ListNode*  list_get_next(List *list){
    if(list->itr)//somewere between head and tail
        list->itr = list->itr->next;
    else { //set it to the head
        list->itr = list->head;
    }
    
    return list->itr;
}
