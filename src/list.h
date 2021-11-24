#ifndef list_included
#define list_included
//----------------------------------------------------------------------

// minimal linked list

//----------------------------------------------------------------------

struct listnode_t {
  void*       data;
  listnode_t* next;
};

//----------

struct list_t {
  listnode_t* head;
  listnode_t* tail;
  int         items;
};

//----------------------------------------------------------------------

list_t* list_create(int initial_size) {
  list_t * list = (list_t*)malloc(sizeof(list_t));
  list->head = NULL;
  list->tail = NULL;
  list->items = 0;
  return list;
}

//----------

void list_destroy(list_t* list, bool free_data=false) {
  if (list) {
    listnode_t* node = list->head;
    while (node) {
      listnode_t* next = node->next;
      if (free_data) free(node->data);
      free(node);
      node = next;
    }
    free(list);
    list->head = NULL;
    list->tail = NULL;
    list->items = 0;
  }
}

//----------

listnode_t* list_append(list_t* list, void* item) {
  listnode_t* node = (listnode_t*)malloc(sizeof(listnode_t));
  node->data = item;
  if (list->tail) {
    // append to existing (tail)
    list->tail->next = node;
    list->tail = node;
    node->next = NULL;
  }
  else {
    // head & tail
    list->head = node;
    list->tail = node;
    node->next = NULL;
  }
  list->items += 1;
  return node;
}

//----------------------------------------------------------------------
#endif
