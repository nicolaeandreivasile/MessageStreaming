#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "list.h"

//~~ List functions ~~//

/* create cell 
 * returns created cell for succes or NULL for failure
 */
list* make_cell(void* x, size_t dim) {

  list* l = (list*) calloc(1, sizeof(list));
  if (!l) {
    return NULL;
  }

  INFO(l) = calloc(1, dim);
  if (!INFO(l)) {
    free(l);
    return NULL;
  }

  memcpy(INFO(l), x, dim);
  NEXT(l) = NULL;

  return l;
}

/* free cell */
void free_cell(list** l) {

  free(INFO(*l));
  free(*l);
}

/* add argument to list
 * returns 0 for succes or 1 if argument is already in list
 */
int add_cell(list** l, void* x, size_t dim, int (*cmp)(void*, void*)) {
  
  list* l1 = (*l);
  list* l2 = l1;

  for (; l1 != NULL; l2 = l1, l1 = NEXT(l1)) {

    if (cmp(INFO(l1), x) == 0) {
      return 1;
    }

    if (cmp(INFO(l1), x) > 0) {
      
      list* to_add_cell = make_cell(x, dim);
      NEXT(to_add_cell) = l1;
      
      if (l2 == l1) {
        (*l) = to_add_cell;
      } else {
        NEXT(l2) = to_add_cell;
      }
      break;
    }
  } 

  if (l1 == NULL) {
    list* to_add_cell = make_cell(x, dim);
  
    if (l2 == l1) {
      (*l) = to_add_cell;
    } else {
      NEXT(l2) = to_add_cell;
    }
  }

  return 0;
}

/* add argument to iend of the list
 * returns 0 for succes or 1 if argument is already in list
 */
int add_last(list** l, void* x, size_t dim) {
  
  list* l1 = (*l);
  list* l2 = l1;

  for (; l1 != NULL; l2 = l1, l1 = NEXT(l1));

  list* to_add_cell = make_cell(x, dim);
  if (l2 == l1) {
    (*l) = to_add_cell;
  } else {
    NEXT(l2) = to_add_cell;
  }

  return 0;
}

/* remove given argumet from list 
 * returns 0 for succes*/
int remove_cell(list** l, void* x, int (*cmp)(void*, void*)) {

  list* l1 = (*l);
  list* l2 = l1;

  for (;l1 != NULL; l2 = l1, l1 = NEXT(l1)) {
    
    if (cmp(INFO(l1), x) == 0) {
      list* to_remove_cell = l1;
      
      if (l2 == l1) {
        (*l) = NEXT(l1);
      } else {
        NEXT(l2) = NEXT(l1);
      }
      free_cell(&to_remove_cell);
    }
  }

  return 0;
}

/* remove given argumet from list 
 * returns 0 for succes*/
int remove_first(list** l) {

  list* l1 = (*l);
  list* l2 = l1;

  list* to_remove_cell = l1;
      
  if (l2 == l1) {
    (*l) = NEXT(l1);
  } else {
    NEXT(l2) = NEXT(l1);
  }

  free_cell(&to_remove_cell);

  return 0;
}

/* check if argument is in list 
 * returns 1 if given argument is found or -1 if it is not found 
 */ 
int contain_cell(list* l, void* x, int (*cmp)(void*, void*)) {

  for (; l != NULL; l = NEXT(l)) {
    
    if (cmp(INFO(l), x) == 0) {
      return 1;
    }
  }

  return -1;
}

/* check if argument is in list
 * return the given cell if it is in list or NULL otherwise
 */
list* get_cell(list* l, void* x, int (*cmp)(void*, void*)) {

  for (; l != NULL; l = NEXT(l)) {
    
    if (cmp(INFO(l), x) == 0) {
      return l;
    }
  }

  return NULL;
}
