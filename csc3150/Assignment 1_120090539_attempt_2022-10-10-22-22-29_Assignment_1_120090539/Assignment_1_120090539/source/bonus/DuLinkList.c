

#include "DuLinkList.h"

void DuLinkListToArr(DuLinkList *list_inStack, DuLinkList_Arr *parr_inStack) {
  // Handling special case
  if (list_inStack->cursize == 0) {
    parr_inStack->arr = NULL;
    parr_inStack->size = 0;
    return;
  }

  parr_inStack->arr = (DuLinkList_ArrNode *)malloc(sizeof(DuLinkList_ArrNode) *
                                                   list_inStack->cursize);
  parr_inStack->size = list_inStack->cursize;
  DuLNode *curNode = list_inStack->head->next;
  for (int i = 0; i < list_inStack->cursize; i++) {
    parr_inStack->arr[i].data = curNode->data;
    parr_inStack->arr[i].pOri = curNode;
    curNode = curNode->next;
  }
}

void ArrToDuLinkList(DuLinkList_Arr *parr_inStack, DuLinkList *list_inStack) {
  // Initialize list_inStack, two side of the head point to itself,cursize==0
  //(break it)
  list_inStack->head->next = list_inStack->head;
  list_inStack->head->prev = list_inStack->head;
  list_inStack->cursize = 0;

  if (parr_inStack->size == 0) {
    // if the length==0==>return
    return;
  }

  for (int i = 0; i < parr_inStack->size; i++) {
    // Insert_Prev(head) insert into the tail. In circular linked list, tail is
    // before the head Break it and connect it
    Insert_Prev(list_inStack, list_inStack->head, parr_inStack->arr[i].pOri);
  }
}

DuLNode *NewNode() // set up
{
  DuLNode *s = (DuLNode *)calloc(1, sizeof(DuLNode)); // calloc-->0
  if (NULL == s) {
    exit(1);
  }
  return s;
}

void FreeNode(DuLNode *p) // release
{
  free(p); // p
}

void InitList(DuLinkList *plist) {
  // It is a Double circular Linked List. When it is initialized it has a head
  // node and the data=0
  assert(plist != NULL);
  DuLNode *s = NewNode(); // data==0
  // the next and prev is point to itself
  s->next = s;
  s->prev = s;
  plist->head = s;
  plist->cursize = 0;
}

void Insert_Prev(
    DuLinkList *plist, DuLNode *target_node,
    DuLNode *pnode) { // The place to insert is in front of the target_node
  assert(plist != NULL && target_node != NULL && pnode != NULL);
  pnode->next = target_node;
  pnode->prev = target_node->prev;
  target_node->prev = pnode;
  pnode->prev->next = pnode;
  plist->cursize += 1;
}

DuLNode *InsertElem_Prev(DuLinkList *plist, DuLNode *target_node,
                         ElemType val) {
  assert(plist != NULL && target_node != NULL);
  DuLNode *s = NewNode();
  s->data = val;
  Insert_Prev(plist, target_node,
              s); // This is why I call it "Insert_Prev"
  return s;
}
void Push_Front(DuLinkList *plist, ElemType val) {
  InsertElem_Prev(plist, plist->head->next, val);
}
DuLNode *Push_Back(DuLinkList *plist, ElemType val) {
  return InsertElem_Prev(plist, plist->head, val);
}
void EarseList(DuLinkList *plist, DuLNode *ptr) {
  assert(plist != NULL && ptr != NULL);
  ptr->next->prev = ptr->prev;
  ptr->prev->next = ptr->next;
  FreeNode(ptr);
  plist->cursize -= 1;
}

void Pop_Back(DuLinkList *plist) {
  assert(plist != NULL);
  if (plist->cursize == 0)
    return;
  EarseList(plist, plist->head->prev);
}
void Pop_Front(DuLinkList *plist) {
  assert(plist != NULL);
  if (plist->cursize == 0)
    return;
  EarseList(plist, plist->head->next);
}

void CloseList(DuLinkList *plist) {
  assert(plist);
  while (plist->head->next != plist->head) {
    DuLNode *q = plist->head->next;
    EarseList(plist, q);
  }
}
void DestroyList(DuLinkList *plist) {
  assert(plist != NULL);
  CloseList(plist);
  FreeNode(plist->head);
  plist->head = NULL;
}

void PrintList(DuLinkList *plist) {
  assert(plist != NULL);
  DuLNode *p = plist->head->next; // frist;
  printf("[start][\n");
  while (p != plist->head) {
    printf("%d ", p->data.pid);
    p = p->next;
  }
  printf("][end]\n");
}
