#ifndef API_LIST_H
#define API_LIST_H

typedef struct list_node_t  {
    struct list_node_t* next;
    void* data;
    
}ListNode;
typedef struct list_t List ;

/*typedef struct iterator_t {
    List* list;
    int   (*hasNext)();
    //void*  (*next)();
}Iterator;*/

//Iterator makeIterator(List* list);
List* addHead(List*, const void* data);
List* addTail(List*, const void* data);
List* listInsertAfter(List* list, ListNode* afterMe, const void* additionData);
List* listInsertBefore(List* list, ListNode* beforeMe, const void* addition);
void* getHead(List*);
void* getTail(List*);


List* removeHead(List* list, int deleteData);
List* removeTail(List* list, int deleteData);

List* listSplitAroundNode(List* orighinal, ListNode* pivot);
List* listSplitAtNode(List* original, ListNode* pivot);
List* makeNewList(int(*deleteObjFunc)(void*) );

void list_reset_iterator(List* list);
int list_has_next(const List *list);
ListNode*  list_get_next(List *list);


List* depleteList(List *list);

int isEmptyList(const List* list);
unsigned listSize(List* list);

List* deleteList(List *list);
#endif