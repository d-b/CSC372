/*
 * CSC372 - Lab 1
 *
 * Process list declarations
 */

struct LL* CreateList(ListType type);
RC DestroyList(struct LL* list);
struct PD* DequeueHead(struct LL* list);
RC PriorityEnqueue(struct PD* pd, struct LL* list);
RC EnqueueAtHead(struct PD* pd, struct LL* list);
RC WaitlistEnqueue(struct PD* pd, int waittime, struct LL* list);
struct PD* FindPD(ProcessId pid, struct LL *list);
RC DequeuePD(struct PD *pd);
struct PD* AllocatePD();
RC DestroyPD(struct PD* pd);
