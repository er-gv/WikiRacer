#ifndef TERMS_DICTIONARY_H
#define TERMS_DICTIONARY_H

#include "term.h"
#include "list.h"
typedef struct tree_node{
    List* keys; //list of keys stored in this tree node
    List* children; //list childe tree nodes.
    unsigned nKeys; //how many items in this tree node
    unsigned isLeaf;
    struct tree_node* parent;
    //struct tree_node* rightmostSubtree;
}TreeNode;

int initDictionary();
int resetDictionary();
int deleteDictionary();

void* findNode(const Term*);
int isExists(const Term*);


void addTermToDictionary(const Term* newTerm);

void printTermsInOrder();
void printTermsLevelOrder();
void printTermsInTreeNode(TreeNode* const node);

#endif