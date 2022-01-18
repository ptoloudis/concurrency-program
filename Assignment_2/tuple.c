#include "mylinda.h"

ts_list_t tuplespace[SIZEOF_TS];

void ts_init(){ // Create and initialize the hash table of tuplespace
  int i;
  for(i=0;i<SIZEOF_TS;i++){
    tuplespace[i].lock = F;
    tuplespace[i].anode = NULL;
  }
}

void free_node(inode_list_t *inode){ // Free the memory of a node
  inode_list_t *nxt;
  for(nxt=inode;nxt->next!=NULL;nxt=nxt->next){ // Free the memory of the node
    free(nxt);
  }
  free(nxt);
}

void ts_destroy(){ // Destroy the hash table of tuplespace
  int i;
  ts_node_t *temp;
  ts_node_t *temp2;

  for(i=0;i<SIZEOF_TS;i++){
    temp = tuplespace[i].anode;
    while(temp != NULL){
      temp2 = temp;
      free_node(temp2->inode);
      temp = temp->next;
      free(temp2);
    }
  }
}

/* In the match function, we check if the tuple mask is a match with the tuple in the tuplespace.
 * If the tuple mask is a match, we return 1, otherwise we return 0.
 * The match function is used in the in and out functions.
 * 
 * Check the type of the tuple mask and the tuple in the tuplespace.
 * Check the all posible cases of the type of the tuple mask and the tuple in the tuplespace.
*/ 

int match(inode_list_t *inode1, inode_list_t *inode2)
{
  inode_list_t *ptr1, *ptr2;

  if ((inode1 == NULL) || (inode2 == NULL)) return 0;

  for (ptr1 = inode1, ptr2 = inode2; ((ptr1 != NULL) && (ptr2 != NULL));
        ptr1 = ptr1->next, ptr2 = ptr2->next) {

    if (ptr1->field_tag == D_FIELD) {
        if (ptr2->field_tag == D_FIELD) {
          switch (ptr1->type_tag) {
            case CHAR_TYPE:
              if ((ptr2->type_tag != CHAR_TYPE) ||
                  (ptr1->node.c != ptr2->node.c)) return 0;
              break;
            case STR_TYPE:
              if ((ptr2->type_tag != STR_TYPE) ||
                  strcmp(ptr1->node.s, ptr2->node.s)) return 0;
              break;
            case INT_TYPE:
              if ((ptr2->type_tag != INT_TYPE) ||
                  (ptr1->node.i != ptr2->node.i)) return 0;
              break;
            case DOUBLE_TYPE:
              if ((ptr2->type_tag != DOUBLE_TYPE) ||
                  (ptr1->node.f != ptr2->node.f)) return 0;
              break;
            default:
              fprintf(stderr, "Shit!\n");
              exit(-1);
              break;
          }
        } else { /* ptr2->field_tag == Q_FIELD */
          switch (ptr1->type_tag) {
            case CHAR_TYPE:
              if (ptr2->type_tag != CHAR_TYPE) return 0;
              break;
            case STR_TYPE:
              if (ptr2->type_tag != STR_TYPE) return 0;
              break;
            case INT_TYPE:
              if (ptr2->type_tag != INT_TYPE) return 0;
              break;
            case DOUBLE_TYPE:
              if (ptr2->type_tag != DOUBLE_TYPE) return 0;
              break;
            default:
              fprintf(stderr, "Shit!\n");
              exit(-1);
              break;
          }
        }
    } else { /* Q_FIELD */
        if (ptr2->field_tag == D_FIELD) {
          switch (ptr1->type_tag) {
            case CHAR_TYPE:
              if (ptr2->type_tag != CHAR_TYPE) return 0;
              break;
            case STR_TYPE:
              if (ptr2->type_tag != STR_TYPE) return 0;
              break;
            case INT_TYPE:
              if (ptr2->type_tag != INT_TYPE) return 0;
              break;
            case DOUBLE_TYPE:
              if (ptr2->type_tag != DOUBLE_TYPE) return 0;
              break;
            default:
              fprintf(stderr, "Shit!\n");
              exit(-1);
              break;
          }
        } else {
          return 0; /* Q_FIELD shouldn't match another Q_FIELD */
        }
    }
  }

  if ((ptr1 == NULL) && (ptr2 == NULL)) return 1;
  else return 0;
}

/* In the bind val fuction we bind the value of the tuple mask to the tuple in the tuplespace.
 * The bind val function is used in the in and out functions.
 *
*/

void bind_val(inode_list_t *inode1, inode_list_t *inode2)
{
    inode_list_t *ptr1, *ptr2;

    if ((inode1 == NULL) || (inode2 == NULL)) return;

    for (ptr1 = inode1, ptr2 = inode2; (ptr1 != NULL) && (ptr2 != NULL);ptr1 = ptr1->next, ptr2 = ptr2->next) {

      if (ptr1->field_tag == D_FIELD) {
        if (ptr2->field_tag == Q_FIELD) {
          switch (ptr1->type_tag) {
          case CHAR_TYPE:
            *ptr2->node.cp = ptr1->node.c;
            break;
          case STR_TYPE:
            strcpy(ptr2->node.s, ptr1->node.s);
            break;
          case INT_TYPE:
            *ptr2->node.ip = ptr1->node.i;
            break;
          case DOUBLE_TYPE:
            *(ptr2->node.fp) = ptr1->node.f;
            break;
          default:
            fprintf(stderr, "Shit!\n");
            exit(-1);
            break;
          }
        } 
      } 
      else { /* Q_FIELD */
        if (ptr2->field_tag == D_FIELD) {
          switch (ptr1->type_tag) {
          case CHAR_TYPE:
            *ptr1->node.cp = ptr2->node.c;
            break;
          case STR_TYPE:
            strcpy(ptr1->node.s, ptr2->node.s);
            break;
          case INT_TYPE:
            *ptr1->node.ip = ptr2->node.i;
            break;
          case DOUBLE_TYPE:
            *ptr1->node.fp = ptr2->node.f;
            break;
          default:
            fprintf(stderr, "Shit!\n");
            exit(-1);
            break;
          }
        }
      }
    }
}

/* In the Out faction we check if the tuple mask is a match with the tuple in the tuplespace.
 * If the tuple mask is a match and the bind val, we remove the tuple from the tuplespace,
 * otherwise we do nothing.
 * 
*/

int out (char *tuple_mask, ...) 
{ 
  va_list tuple_list; 
  char *mask_ptr; 
  int len, hash, i;
  ts_node_t *tsnode, *tsptr;
  inode_list_t *inode , *node_ptr;


  len = (strlen(tuple_mask) / 2);
  hash = 0;

  tsnode = (ts_node_t *)(malloc(sizeof( ts_node_t)));
  tsnode->type = OUT;
  tsnode->cv = F;
  tsnode->inode = NULL;
  tsnode->next = NULL;
  tsnode->prev = NULL;

  node_ptr = NULL;

  va_start(tuple_list, tuple_mask); 

  for (mask_ptr = tuple_mask, i = 1; *mask_ptr; mask_ptr+=2, i++) 
  { 
    if (*mask_ptr == '%') 
    {
      switch (mask_ptr[1]) {
        case 's': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.s = va_arg(tuple_list, char *); 
          inode->type_tag = STR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 3 * i;
          break;
        case 'd': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.i = va_arg(tuple_list, int); 
          inode->type_tag = INT_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 5 * i;
          break;
        case 'f': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.f = va_arg(tuple_list, double); 
          inode->type_tag = DOUBLE_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 7 * i;
          break;
        case 'c': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.c = (char) va_arg(tuple_list, int); 
          inode->type_tag = CHAR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 11 * i;
          break;
        default:
          fprintf(stderr, "Unknown type of an out.\n");
          exit(-1);
        }  
    } 
    else {
      if (*mask_ptr == '?') {
        fprintf(stderr, "Cannot do a '?' on a out!");
        exit(-1);
      }
    }
      
    if (tsnode->inode == NULL) {
      tsnode->inode = node_ptr = inode;
    } 
    else {
      node_ptr->next = inode;
      node_ptr = inode;
    }
    
  } 

  va_end(tuple_list); 

  /* Enter it into the tuple space */

  len = (len + hash) % SIZEOF_TS;
  while (tuplespace[len].lock == T){  }// Wait to enter the tuple space
  
  tuplespace[len].lock = T;
  
  for (tsptr = tuplespace[len].anode; tsptr != NULL; tsptr = tsptr->next) {

    if (match(tsnode->inode, tsptr->inode)) {
      if (tsptr->type == IN) {
        bind_val(tsnode->inode, tsptr->inode);
        tsptr->cv = T;
        tuplespace[len].lock = F;
        free_node(tsnode->inode);
        free(tsnode);
        return 0;
      }
    }
  }

  if (tuplespace[len].anode != NULL) {
    tuplespace[len].anode->prev = tsnode;
  }
  tsnode->next = tuplespace[len].anode;
  tuplespace[len].anode = tsnode;
  tuplespace[len].lock = F;
  return 0;
}

/* In the In faction we check if the tuple mask is a match with the tuple in the tuplespace.
 * If the tuple mask is a match and the bind val, we remove the tuple from the tuplespace,
 * Else wait to be notified.
 *
*/

int in (char *tuple_mask, ...) { 
  va_list tuple_list; 
  char *mask_ptr; 
  int len, hash, i;
  inode_list_t *inode, *node_ptr;
  ts_node_t *tsnode, *ptsptr, *tsptr;

  len = (strlen(tuple_mask) / 2);
  hash = 0;
  
  tsnode = (ts_node_t *)(malloc(sizeof(ts_node_t)));
  tsnode->type = IN;
  tsnode->cv = F;
  tsnode->inode = NULL;
  tsnode->next = NULL;
  tsnode->prev = NULL;

  node_ptr = NULL;

  va_start(tuple_list, tuple_mask); 

  for (mask_ptr = tuple_mask, i = 1; *mask_ptr; mask_ptr+=2, i++) 
  { 
    if (*mask_ptr == '%') 
    {
      switch (mask_ptr[1]){ 
        case 's': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.s = va_arg(tuple_list, char *); 
          inode->type_tag = STR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 3 * i;
          break;
        case 'd': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.i = va_arg(tuple_list, int); 
          inode->type_tag = INT_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 5 * i;
          break;
        case 'f': 
          inode =  (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.f = va_arg(tuple_list, double); 
          inode->type_tag = DOUBLE_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 7 * i;
          break;
        case 'c': 
          inode =  (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.c = (char) va_arg(tuple_list, int); 
          inode->type_tag = CHAR_TYPE;
          inode->field_tag = D_FIELD;
          inode->next = NULL;
          hash += 11 * i;
          break;
        default:
          fprintf(stderr, "Unknown type of an out.\n");
          exit(-1);
        } 
    } else {
      if (*mask_ptr == '?') 
      {
        switch (mask_ptr[1]){ 
        case 's': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.s = va_arg(tuple_list, char *); 
          inode->type_tag = STR_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 3 * i;
          break;
        case 'd': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.ip = va_arg(tuple_list, int *); 
          inode->type_tag = INT_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 5 * i;
          break;
        case 'f': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
          inode->node.f = va_arg(tuple_list, double); 
          inode->type_tag = DOUBLE_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 7 * i;
          break;
        case 'c': 
          inode = (inode_list_t *)(malloc(sizeof(inode_list_t)));
         inode->node.c = (char) va_arg(tuple_list, int); 
          inode->type_tag = CHAR_TYPE;
          inode->field_tag = Q_FIELD;
          inode->next = NULL;
          hash += 11 * i;
          break;
        default:
          fprintf(stderr, "Unknown type of an in.\n");
          exit(-1);
        }
      } else {
          fprintf(stderr, "Don't know what it is - in ?!");
          exit(-1);
      }
    }

    if (tsnode->inode == NULL) {
      tsnode->inode = node_ptr = inode;
    } else {
      node_ptr->next = inode;
      node_ptr = inode;
    }
  } 

  va_end(tuple_list); 

  /* Attempt to find it in the tuple space */

  len = (len + hash) % SIZEOF_TS;
  tuplespace[len].lock = T;
  
  ptsptr = NULL;

  for (tsptr = tuplespace[len].anode; tsptr != NULL; tsptr = tsptr->next) {

    if (match(tsnode->inode, tsptr->inode)) {
      if (tsptr->type == OUT) {
        bind_val(tsnode->inode, tsptr->inode);

        /* Delink the out tuple */
        if (ptsptr == NULL) {
          tuplespace[len].anode = tsptr->next;
        } else {
          ptsptr->next = tsptr->next;
        }

        tuplespace[len].lock = F;
        free_node(tsnode->inode);
        free_node(tsptr->inode);
        free(tsptr);
        free(tsnode);

        return 2;
      } 
    }

    ptsptr = tsptr;
  }

  if (tuplespace[len].anode != NULL) {
    tuplespace[len].anode->prev = tsnode;
  }
  tsnode->next = tuplespace[len].anode;
  tuplespace[len].anode = tsnode;
  
  tuplespace[len].lock = F;
  while (tsnode->cv == F){}
  while (tuplespace[len].lock == T){}
  tuplespace[len].lock = T;
  
  /* Delink the out tuple */
  if (tsnode->prev == NULL) {
    tuplespace[len].anode = tsnode->next;
    if (tsnode->next != NULL) {
      tsnode->next->prev = NULL;
    }
  } else {
    tsnode->prev->next = tsnode->next;
  }
  free_node(tsnode->inode);
  free(tsnode);
  tuplespace[len].lock = F;
  return 1;
}