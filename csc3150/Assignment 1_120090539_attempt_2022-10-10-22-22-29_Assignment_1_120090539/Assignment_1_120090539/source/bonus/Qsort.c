#include "Qsort.h"
#include "DuLinkList.h"

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

int partition(DuLinkList_ArrNode *arr, int left, int right) {
  DuLinkList_ArrNode pivot = arr[left];
  // At this time the left leave a seat vacant for others
  while (1) {
    // right   Since the left leave a seat vacant for others right now. As a
    // result we need to move "right" first until find a number smaller than the
    // pivot
    while (1) {
      ///[The key point of logical sort(1)] if (arr[right].data.pid <
      ///pivot.data.pid)
      if (arr[right].data.pid <
          pivot.data.pid) { // We do not need to use <= since we do not need to
                            // move the equal on, which is faster
        arr[left] = arr[right];
        left++;
        if (left == right) {
          arr[left] = pivot;
          return left;
        } // Returns the final position of the pivot
        break;
      } else { // arr[right] > pivot
        right--;
        if (left == right) {
          arr[left] = pivot;
          return left;
        } // Returns the final position of the pivot
      }
    }
    // left Since the right leave a seat vacant for others right now. As a
    // result we need to move "left" first until find a number smaller than the
    // pivot
    while (1) {
      ///[The key point of logical sort(2)]if (arr[left].data.pid >
      ///pivot.data.pid)
      if (arr[left].data.pid >
          pivot.data.pid) { // The reason why we do not use the >= is similar to
                            // the above
        arr[right] = arr[left];
        right--;
        if (left == right) {
          arr[left] = pivot;
          return left;
        } // Returns the final position of the pivot
        break;
      } else { // arr[right] > pivot
        left++;
        if (left == right) {
          arr[left] = pivot;
          return left;
        } // Returns the final position of the pivot
      }
    }
  }
}

void _Qsort(DuLinkList_ArrNode *arr, int left, int right) {
  int pivotPos = partition(arr, left, right);
  // left~pivotPos-1 pivotPos+1~right
  if (left <
      pivotPos -
          1) // At least two or more out-of-order element, need to partition
  {
    _Qsort(arr, left, pivotPos - 1);
  } else {
    // left==pivotPos-1 There is only one number in the left interval no need to
    // partition left==pivotPos  No number in the left interval no need to
    // partition we just skip
  }

  if (pivotPos + 1 <
      right) // At least two or more out-of-order element, need to partition
  {
    _Qsort(arr, pivotPos + 1, right);
  } else {
    // right==pivotPos-1 There is only one number in the right interval no need
    // to partition right==pivotPos  No number in the right interval no need to
    // partition we just skip
  }
}

void Qsort(DuLinkList *list, int size) {
  if (size <= 1)
    return;

  DuLinkList_Arr dularr;
  DuLinkListToArr(list, &dularr);

  _Qsort(dularr.arr, 0, size - 1);

  ArrToDuLinkList(&dularr, list);

  free(dularr.arr);
}
