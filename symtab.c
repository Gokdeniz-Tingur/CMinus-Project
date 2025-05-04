
/*************************************************************/
/*   File: symtab.c                                          */
/*   Symbol table implementation for the C-Minus compiler    */
/*   (allows only one symbol table)                          */
/*                                                           */
/*   This file needs to be completed as part of your project4*/
/*************************************************************/

#include "symtab.h"


/* SIZE is the size of each hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
    //(1): add code here to complete the implementation of the hash function
  int i = 0;

  while (key[i] != '\0') {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    i++;
  }
  return temp;
}

/* The record in the bucket lists for
 * each defnode
 */
typedef struct BucketListRec
   { TreePtr defnode;
     struct BucketListRec * next;
   } * BucketList;

/* each hash table */
typedef BucketList Table[SIZE];

/* the linked list of tables */
typedef struct TableListRec
   { Table t;
     struct TableListRec * next;
   } * TableList;

/* The actual symbol table */
static TableList symTab = NULL; /* starts out empty */
static int currentNestLevel = 0;

/* Procedure st_enterScope starts a new scope
 * returns 0 if memory allocation fails, else 1
 */
int st_enterScope(void)
{ TableList tempTab = (TableList) malloc(sizeof(struct TableListRec));
  int i;
  if (!tempTab) return 0; /* memory allocation error */
  for (i=0;i<SIZE;i++) tempTab->t[i] = NULL;
  tempTab->next = symTab;
  symTab = tempTab;
  return 1;
} /* st_enterScope */

/* Procedure st_exitScope removes all declarations
 * in the current scope
 */
void st_exitScope(void)
{ 

	//(2): Add code to handle the case when a scope needs to be deleted
  if (symTab != NULL) {
    TableList oldScope = symTab;
    symTab = symTab -> next;

    for (int i = 0; i < SIZE; i++) {
      BucketList current = oldScope -> t[i];
      while (current != NULL) {
        BucketList temp = current;
        current = current -> next;
        free(temp);
      }
    }

    free(oldScope);
    currentNestLevel--;

    printf("Existing scope and decrementing nestLevel\n");
    printf("New nesting level = %d\n", currentNestLevel);
  }
}

/* Procedure st_insert inserts def nodes from
 * from the syntax tree into the symbol table
 * returns 0 if memory allocation fails, else 1
 */
int st_insert( TreePtr t)
{ int h = hash(t->attr.name);
  BucketList l;
  if (!symTab) st_enterScope();
  if (!symTab) return 0; /* memory allocation error */
  l = (BucketList) malloc(sizeof(struct BucketListRec));
  if (!l) return 0; /* memory allocation error */
  l->defnode = t;
  l->next = symTab->t[h];
  symTab->t[h] = l;
  return 1;
}

TreePtr st_lookup ( char * name )
{ 

  //(3) Add code to handle the case of lookup process
  //3-1 calc the hash value of the name
  int h = hash(name);

  //3-2 create a temp variable (TableList) and use it to scan through the whole table looking for a match 
  TableList currentTable = symTab;

  //3-3 while loop to go through the table
  while(currentTable != NULL) {
  //       find each hash table content - should be a BucketList
    BucketList currentBucket = currentTable -> t[h];
    //       compare the name of the content with the name looking for (defnode->arrt.name)
    //       if name is found, return the defnode of the current BucketList
    //               else move on to the next table content
    while (currentBucket != NULL) {
      if (strcmp(name, currentBucket -> defnode -> attr.name) == 0) {
        printf("Lookup of identifier %s at line %d\n", name, currentBucket -> defnode -> lineno);
        printf("Found on line %d\n", currentBucket -> defnode -> lineno);
        return currentBucket -> defnode;
      }
      currentBucket = currentBucket -> next;
    }
    currentTable = currentTable -> next;
  }
  //3-4 return NULL if not found; 
  return NULL;
}  /* st_lookup */


void printSymTab(FILE * listing) {
  
}