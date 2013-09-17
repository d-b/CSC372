//INCOMPLETE AND UNRUNNALBE

#include "list.h"
#include "process.h"
#include <iostream>

int main() {
  //Create 3 processes 
  struct LL * list = CreateList(L_PRIORITY);
  struct PD* pd1 = AllocatePD();
  pd1->priority = 4;
  pd1->pid = 1;
  
  struct PD* pd2 = AllocatePD();
  pd1->priority = 2;
  pd1->pid = 2;
  
  struct PD* pd3 = AllocatePD();
  pd1->priority = 6;
  pd1->pid = 3;
  
  //Insert into list => [pd2, pd1, pd3]
  PriorityEnqueue(pd1, list);
  PriorityEnqueue(pd2, list);
  PriorityEnqueue(pd3, list);

  //Assert position of processes are correct
  assertSame(FindNth(0), pd2);
  assertSame(FindNth(1), pd1);
  assertSame(FindNth(2), pd3);
  
  //Assert length of list is correct
  assertEqual(3, FindLength(list));
  
  //Assert that pd2 can be found using FindPD
  assertSame(pd2, FindPD(2));
  
  //Assert that pd2 (the smallest priority) is returned when dequeue is called
  assertSame(DequeueHead(list), pd2);
  //Assert length has decreased by 1
  assertEqual(2, FindLength(list));
  
  //Assert pd2 is no longer paft of the list
  assertSame(NULL, FindPD(2));
  
  //Dequeue pd 3, assert pd3 can no longer be found and list has decreased by 1
  DequeuePD(pd3);
  assertSame(NULL, FindPD(3));
  assertEqual(1, FindLength(list));
  
  //Assert that pd1 is returned
  assertSame(DequeueHead(list), pd1);
  //Assert list is empty
  assertEqual(0, FindLength(list));

  //Assert NULL is returned when dequeueing empty list
  assertSame(NULL, DequeueHead(list));

  return(0);
}

struct PD* FindLength(struct LL* list) {
  int i = 0;
  struct PD* cur = list->head;
  while(cur) {
    cur = cur->link;
    i++;
  }
}

struct PD* FindNth(int n, struct LL* list) {
  int i = 0;
  
  struct PD* cur = list->head;
  
  while(cur) {
    if (i == n) {
      return(cur);
    }
    
    cur = cur->link;
    i++;
  }
  return NULL;
}

void assertSame(void * eq1, void * eq2) {
  if (eq1 != eq2) 
    exit(-1);
}

void assertEqual(int i1, int i2) {
  if (i1 != i2)
    exit(-1);
}