#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "terms_dictionary.h"
//#include "list.h"
#include "error_codes.h"






static int deleteTreeNode(TreeNode* node);

static void addNodeNonefull(TreeNode* node, const Term* newTermStr);
static ListNode* insertKeyInOrder(TreeNode* , const Term* );
static TreeNode* split(TreeNode* nodeToSplit, TreeNode* hisParent, unsigned childIndex );
static void printSubtreeLevelOrder(TreeNode* subtree);
//every node hold at most 2(balance_factor) child nodes and 2(bf)-1 keys
//every node except the root contains at least 1(balance_factor) keys
static unsigned balance_factor = 5; 
static TreeNode* termsDictionary = NULL;

static TreeNode* makeNewTreeNode(){
    
    TreeNode* node = malloc(sizeof(TreeNode));
    if(node){
        
        node->keys = makeNewList((int(*)(void*))removeTerm);
        node->children = makeNewList((int(*)(void*))deleteTreeNode);
        node->nKeys = 0;
        //node->isLeaf should set by the caller
        //node->parent should set by the caller
        return node;
    }
    else
        return NULL;
}


int initDictionary(int balanceFactor){
    if(balanceFactor<2){
        //assert that balanceFactor >=2
        balanceFactor = 2;
    }
    balance_factor = balanceFactor;
    termsDictionary = makeNewTreeNode();
    
    if(termsDictionary){
        termsDictionary->isLeaf = 1u;
        termsDictionary->parent = NULL;
        return 0;
    }
    return -1;
    
}

static int isFullNode(const TreeNode* node){
    return node->nKeys == (2*balance_factor -1);
}


int isExists(const Term* searchTerm /*char* newTermStr*/){
      
    TreeNode* treeNode = termsDictionary;
    ListNode* subTreePtr = NULL;
    int found = 0;
    int cmp;
    List* keysInCurrentNode;
    if(!treeNode || !treeNode->keys)
        return 0;
    
    //keysInCurrentNode = treeNode->keys; //should never be an empty items_list
    
    
    while(!found && treeNode){
        list_reset_iterator(treeNode->keys);
        list_reset_iterator(treeNode->children);
        do{
        
            ListNode* listNode = list_get_next(treeNode->keys);
            Term* term = (Term*)(listNode->data);    
            
            subTreePtr = list_get_next(treeNode->children);
            cmp = cmpTerms(searchTerm, term);
        }while( cmp>0 && list_has_next(treeNode->keys));    
        
        if(cmp == 0){
            found = 1;
        }
        else if(cmp <0){
            if(subTreePtr){
                treeNode = (TreeNode*)subTreePtr->data;
            }
            else{
                treeNode = NULL;
            }
            
        }
        else{
            subTreePtr = list_get_next(treeNode->children);
            if(subTreePtr){
                treeNode = (TreeNode*)subTreePtr->data;
            }
            else{
                treeNode = NULL;
            }
        }
        
    }//while
           
       
    return found;
    
}


void* findNode(const Term* searchTerm /*char* newTermStr*/){
      
    
    TreeNode* resultObj = NULL;
    TreeNode* treeNode = termsDictionary;
    int found = 0;
    List* keysInCurrentNode;
    
    if(!treeNode || !treeNode->keys)
        return 0;
    
    keysInCurrentNode = treeNode->keys; //should never be an empty items_list
    list_reset_iterator(keysInCurrentNode);
    list_reset_iterator(treeNode->children);
    do{
        while(!found && treeNode!= NULL && list_has_next(treeNode->keys)){
            Term* term = (Term*)(list_get_next(treeNode->keys));    
            TreeNode* childNodeWithSmallerKeys = (TreeNode*)list_get_next(treeNode->children);
            int cmp = strcmp(term->text, searchTerm->text);
            if(cmp == 0){
                resultObj = treeNode;                
            }
            
            else if(cmp> 0){
                treeNode = childNodeWithSmallerKeys;
                keysInCurrentNode = treeNode->keys;
                list_reset_iterator(keysInCurrentNode);
                list_reset_iterator(treeNode->children);
                
            }
            //else: the search key is larger then current. need to look for further in the key list or sub trees.
        }//while
        //we got to the end of
        if(!found){
            treeNode = (TreeNode*)(getTail(treeNode->children));
        }
    }while(!found && treeNode!= NULL);
    
    return resultObj;
    
}


static ListNode* insertKeyInOrder(TreeNode* treeNode, const Term* term){
//adds new KeyItem object into the TreeNode's list of keys, maintaing the ascending order of the keys    
    //puts("@insertKeyInOrder");
    ListNode* ret_val;
    if(isEmptyList(treeNode->keys)){
        treeNode->keys = addHead(treeNode->keys, term);
        ret_val = getHead(treeNode->keys);
        //treeNode->children = addHead(treeNode->children, NULL);
    }
    else{
        
        list_reset_iterator(treeNode->keys);
        //ListNode* prev = NULL;
        int found = 0;
        ListNode* current;
        //puts("else");
        do{
            current = list_get_next(treeNode->keys);
            Term* existingTerm = (Term*)current->data;
            
            if(cmpTerms(term, existingTerm)<=0)
                found = 1;
            
            
        }while(!found && list_has_next(treeNode->keys));
        
        if(found){
            treeNode->keys = listInsertBefore(treeNode->keys, current, (void*)term);
         //   treeNode->children = listInsertBefore(treeNode->children, NULL);
        }
        else{
            treeNode->keys = listInsertAfter(treeNode->keys, current, (void*)term);
           // treeNode->children = listInsertAfter(treeNode->children, NULL);
        }
        ret_val = current->next;
    }
   
    //puts("leaving insertKeyInOrder");
    return ret_val;
    
}

void addTermToDictionary(const Term* newTerm){
    //puts("@addTerm.\n");
    //printTerm(newTerm);
    if(isFullNode(termsDictionary)){
        //puts("Current node is full and requires split");
        TreeNode* newNode = makeNewTreeNode(); //make new root node for the tree.
        //and set the current root to be its leftmost child.
        //TreeNode* nodeToSplit = termsDictionary;// remember original root for the call to split()
        
        newNode->isLeaf = 0u;
        newNode->nKeys = 0u;
        //printf("addind term ");
        //printTerm(newTerm);
        
        //printf("Will split tree root. before splitting the tree is:\n");
        //printSubtreeLevelOrder(termsDictionary);
        
        //old root become leftmost child of new root
        
        newNode->children = addHead(newNode->children, termsDictionary);
        termsDictionary->isLeaf = 1u;
        
        //split 
        split(termsDictionary, newNode, 0u);
        termsDictionary =newNode;
        termsDictionary->isLeaf = 0u;
        //printf("after splitting the root the tree is:\n");
        //printSubtreeLevelOrder(termsDictionary);
        addNodeNonefull(termsDictionary, newTerm);
        //printf("and after adding the term %s the root the tree is:\n", newTerm->text);
        //printSubtreeLevelOrder(termsDictionary);       
    }
    else{
       //puts("Adding term to nonfull node...");
       addNodeNonefull(termsDictionary, newTerm);
    }
    //puts("Leave addTerm");
}


static void addNodeNonefull(TreeNode* treeNode, const Term* newTerm){
    
    //puts("@addNodeNonefull. adding term:");
    //printTerm(newTerm);
    //First we find after which key in this node to put the new key
        
    if(treeNode->isLeaf){
        //add the new term  to this treeNode, keep the ascending order
        
        //puts("deal with leaf node...");
         //printSubtreeLevelOrder(treeNode);       
        //insert into the appropriate position in the keys list
        insertKeyInOrder(treeNode, newTerm);
        treeNode->nKeys++;
    }
    else{
        //puts("dealing with node that has children");
        list_reset_iterator(treeNode->keys);
        list_reset_iterator(treeNode->children);
        int found = 0;
        unsigned index =0;
        Term* term;
        TreeNode* child;
        while(!found && list_has_next(treeNode->keys)){
            ListNode* node = list_get_next(treeNode->keys);
            term = (Term*)(node->data);
            node = list_get_next(treeNode->children);    
            child = (TreeNode*)(node->data);
            index++;
            if(cmpTerms(newTerm, term) <0){
      //          printf("newTerm->text == %s is smaller then term->text==%s", newTerm->text, term->text);
                found = 1;                
            }
        }
        //printf("index counted for %d.\n", index);
        if(!found){
            //the new term should be place
            //puts("add term after largest key in node");
            ListNode* node = list_get_next(treeNode->children);    
            child = (TreeNode*)(node->data);
        }
        //puts("Found position in nonfull treeNode");
        if(child && isFullNode(child)){
            //printf("will split child node. pivot goes to parent at index %d.\n", index);
            //printf("before splitting the parent had %d keys and %d children.\n",listSize(treeNode->keys), listSize(treeNode->children));
            split(child, treeNode, index);
            //printf("after splitting the parent it has %d keys and %d children.\n",listSize(treeNode->keys), listSize(treeNode->children));
            addNodeNonefull(treeNode, newTerm);
        }
        else{
           // puts("go recursion go..");
            addNodeNonefull(child, newTerm);
        }
    }
        
    //puts("leaving addNodeNonefull");
}//addNodeNoneful


static TreeNode* split(TreeNode* nodeToSplit, TreeNode* hisParent, unsigned childIndex){
    
    //make a new child node
    TreeNode* newNode = makeNewTreeNode();
    //puts("@split.");
    
    //look for the median key inside nodeToSplit's keys list. 
    //we gonna split this List into two List objects, returning the later half.
   
    
    ListNode* keysPivot;
    ListNode* childrenPivot;
    
    //lets split nodeToSplit
    
    list_reset_iterator(nodeToSplit->keys);
   
    for(int j=0; j<balance_factor; j++){
        keysPivot = list_get_next(nodeToSplit->keys);
    }
    //split the keys list around keysPivot. The prefix ends onde before the pivot and the suffix starts one node after it
    List* keysForNewNode = listSplitAroundNode(nodeToSplit->keys, keysPivot);
    //@@@@@@@@@@
    //addPivotKeyAndSplitedNode(hisParent, keysPivot, newNode);
    insertKeyInOrder(hisParent, (Term*)(keysPivot->data));
    //ListNode* listNode=NULL;
    
    list_reset_iterator(hisParent->children);
    for(int j=0; j<childIndex; j++){
        childrenPivot =list_get_next(hisParent->children);
    }
    childrenPivot =list_get_next(hisParent->children);
    
    listInsertAfter(hisParent->children, childrenPivot, newNode);
    newNode->isLeaf = nodeToSplit->isLeaf;
    newNode->nKeys = balance_factor -1;
    nodeToSplit->nKeys = balance_factor -1;
    hisParent->nKeys++;
    
    //spliting the children to exct two lists, no node left out
    
    
    if(!isEmptyList(nodeToSplit->children)){
        //puts("@@@@@@@@@@@@@@@@@@@2will split children@@@@@@@@@@@@@22");
        
        //printf("node has %d children.\n", listSize(nodeToSplit->children));
        list_reset_iterator(nodeToSplit->children);
        for(int j=0; j<balance_factor; j++){
            childrenPivot =list_get_next(nodeToSplit->children);
        }
        childrenPivot =list_get_next(nodeToSplit->children);
        deleteList(newNode->children);//remove the existing (empty) children list befor adding new one
        List* childrenForNewNode = listSplitAtNode(nodeToSplit->children, childrenPivot);
        //printf("after split, prefix has %d children and suffix has %d.\n", listSize(nodeToSplit->children), listSize(childrenForNewNode));
       
        newNode->children = childrenForNewNode;
        newNode->isLeaf = 0u;
        
    }
    
        
    newNode->keys = keysForNewNode;
   
    return newNode;
}


void printTermsInOrder(){
    
}


void printTermsInTreeNode(TreeNode* const node){
    
    ListNode* item;
    list_reset_iterator(node->keys);
    while(item = list_get_next(node->keys)){
        Term* term = (Term*)item->data;
        printTerm(term);
        puts("->");
    }
}

void printSubtreeLevelOrder(TreeNode* subtree){
    List* queue = makeNewList((int(*)(void*))deleteTreeNode);
    if(subtree)
        addHead(queue, subtree);
    printf("@printSubtreeLevelOrder\n");
    while(!isEmptyList(queue)){
        TreeNode* currentNode = getTail(queue);
        
        List* keysInNode = currentNode->keys;
        if(!isEmptyList(keysInNode)){
            printf("tree node has %d keys and %d children:\n", listSize(currentNode->keys), listSize(currentNode->children));
            list_reset_iterator(keysInNode);
            while(list_has_next(keysInNode)){
                Term* key = (Term*)(list_get_next(keysInNode)->data);
                printTerm(key);
            }
            list_reset_iterator(currentNode->children);
            while(list_has_next(currentNode->children)){
                puts("inserting new tree node to the print queue");
                TreeNode* child = (TreeNode*)(list_get_next(currentNode->children)->data);
                queue = addHead(queue, child);
            }
            
        }
        puts("");
        //puts("removing a tree node from the print queue");
        queue = removeTail(queue, 0);
    }
    puts("DONE printSubtreeLevelOrder");
}

void printTermsLevelOrder(){
    printf("@printTermsLevelOrder:\n");
    printSubtreeLevelOrder(termsDictionary);
}


int deleteTreeNode(TreeNode* node){
    //puts("@deleteTreeNode");
    deleteList(node->keys);
    deleteList(node->children);
    //puts("calling free() on this treeNode");
    free(node);
    return 0;
}


int resetDictionary(){
    deleteList(termsDictionary->keys);
    deleteList(termsDictionary->children);
    termsDictionary->nKeys = 0;
    termsDictionary->isLeaf = 1;
    return 0;
}

int deleteDictionary(){
    puts("@deleteDictionary");
    if(termsDictionary){
        deleteTreeNode(termsDictionary);
        termsDictionary = NULL;
    }
    return 0;
}
