#ifndef _DULINKLIST_H_
#define _DULINKLIST_H_

#include <assert.h>
#include <stdio.h>
#include <string.h>

//================================================
typedef struct _DuLNode DuLNode; // I dont know whether I must add this type. It
                                 // will be err if I do not add it,sos

typedef struct _DuLinkList {
  DuLNode *head;
  int cursize;
} DuLinkList;

#include "main.h"

typedef struct _PsTreeNode ElemType;

typedef struct _DuLNode {
  ElemType data;
  struct _DuLNode *prev;
  struct _DuLNode *next;
} DuLNode, *PDuLNode;

//================================================
typedef struct _DuLinkList_ArrNode {
  ElemType data;
  DuLNode *pOri;
} DuLinkList_ArrNode;

typedef struct _DuLinkList_Arr {
  DuLinkList_ArrNode *arr;
  int size;
} DuLinkList_Arr;

//==============================================================================
void DuLinkListToArr(DuLinkList *list_inStack, DuLinkList_Arr *parr_inStack);

void ArrToDuLinkList(DuLinkList_Arr *parr_inStack, DuLinkList *list_inStack);

DuLNode *NewNode();

void FreeNode(DuLNode *p);

void InitList(DuLinkList *plist);

void Insert_Prev(DuLinkList *plist, DuLNode *pre, DuLNode *pnode);

DuLNode *InsertElem_Prev(DuLinkList *plist, DuLNode *pre, ElemType val);

void Push_Front(DuLinkList *plist, ElemType val);

DuLNode *Push_Back(DuLinkList *plist, ElemType val);

void EarseList(DuLinkList *plist, DuLNode *ptr);

void Pop_Back(DuLinkList *plist);

void Pop_Front(DuLinkList *plist);

void CloseList(DuLinkList *plist);

void DestroyList(DuLinkList *plist);

void PrintList(DuLinkList *plist);

#endif // _DULINKLIST_H_