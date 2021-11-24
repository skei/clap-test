#ifndef array_included
#define array_included
//----------------------------------------------------------------------

// minimal dynamic array..
// doubles inn size (uses realloc) when append_item reaches already
// allocated size

//----------------------------------------------------------------------

struct array_t {
  void**  buffer;     // buffer
  int     buffersize; // size of buffer (num items)
  int     items;      // number of used/occupied items in buffer
};

//----------------------------------------------------------------------

array_t* array_create(int initial_size) {
  array_t* array = (array_t*)malloc(sizeof(array_t));
  if (array) {
    array->buffersize = initial_size;
    array->buffer = (void**)malloc(initial_size * sizeof(void*));
    array->items = 0;
  }
  return array;
}

//----------

void array_destroy(array_t* array) {
  int i;
  if (array) {
    for (i=0; i<array->items; i++) {
      if (array->buffer[i]) {
        free(array->buffer[i]);
        array->buffer[i] = NULL;
      }
    }
    free(array);
  }
}
//----------

int array_append(array_t* array, void* item) {
  int index = array->items;
  if (array->items >= array->buffersize) {
    array->buffersize *= 2;
    array->buffer = (void**)realloc(array->buffer,array->buffersize * sizeof(void*));
  }
  array->buffer[array->items] = item;
  array->items += 1;
  return index;
}

//----------------------------------------------------------------------
#endif
