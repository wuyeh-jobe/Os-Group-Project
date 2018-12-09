#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define HASH_SIZE 20
// basic node structure
typedef struct _node_l {
  char *value;
  struct _node_l   *next;
} node_l;

// linked list structure
typedef struct _list_l {
  node_l *head;
  int count;
  pthread_mutex_t lock;
} list_l;

// initialize list lock
pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
// initialize hash table 
list_l *hashtable[HASH_SIZE] = {0};
// define the hash function
size_t hash(const char *str) {
  size_t hash = 5381;
  for (int i = 0; str[i] != '\0'; i++) {
    hash += hash * 20 + (int) str[i];
  }
  return hash % HASH_SIZE;
}
struct DataItem {
   int data;   
   int key;
   
};
struct DataItem* hashArray[HASH_SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

int hashCode(int key) {
   return key % HASH_SIZE;
}
// initialize a list
list_l* List_Init() {
  list_l *new_list = malloc(sizeof (list_l));
  if (new_list == NULL) {
    fprintf(stderr, "Cannot allocate memory for new list.\n");
  }
  // initialize head and lock
  new_list->head = NULL;
  new_list->count = 0;
  pthread_mutex_init(&(new_list->lock), NULL);
  return new_list;
}
// create a new node
node_l* newNode(char *value) {
  node_l *new_node = malloc(sizeof (node_l));
  if (new_node == NULL) {
    fprintf(stderr, "Cannot allocate memory for new node.\n");
  }
  new_node->value = malloc(sizeof (strlen(value) + 1));
  if (new_node->value == NULL) {
    fprintf(stderr, "Cannot allocate memory for value.\n"); 
  }
  //copy value into node
  strcpy(new_node->value, value);
  new_node->next = NULL;
  return new_node;
}

//look up
char* List_Lookup(const char *check) {
  size_t index = hash(check);
  // check if list exists
  if (hashtable[index] == NULL) {
    return NULL;
  }
  // Lock at hashtable index
  pthread_mutex_lock(&(hashtable[index]->lock));
  // Get list head at hashtable[index] 
  node_l *checker = hashtable[index]->head;
  // Find value in list
  while (checker) {
    if (strcasecmp(check, checker->value) == 0) {
      pthread_mutex_unlock(&(hashtable[index]->lock));
      // if found value
      return checker->value;
    }
    checker = checker->next;
  }
  pthread_mutex_unlock(&(hashtable[index]->lock));
  // if value not find return null
  return NULL;
}
//insert node
int List_Insert(char *value) {
  node_l *new_node = newNode(value);
  // Get hash index
  size_t index = hash(value);
  // Initialize list at hash table index
  if (hashtable[index] == NULL) {
    pthread_mutex_lock(&(list_lock));
    if (hashtable[index] == NULL) {
      list_l *new_list = List_Init();
      hashtable[index] = new_list;
    }
    pthread_mutex_unlock(&(list_lock));
  } 
  // Lock critical section
  pthread_mutex_lock(&(hashtable[index]->lock));
  new_node->next = hashtable[index]->head; 
  hashtable[index]->head = new_node;
  hashtable[index]->count++;
  pthread_mutex_unlock(&(hashtable[index]->lock));
  return 0;
}

// delete node
int List_Delete(const char *value) {
  size_t index = hash(value);
  // Lock at hashtable index
  pthread_mutex_lock(&(hashtable[index]->lock));
  // Get list head at hashtable index
  node_l *current = hashtable[index]->head;
  node_l *previous = NULL;
  while (current) {
    if (strcasecmp(value, current->value) == 0) {
      // Value is first item in list
      if (current == hashtable[index]->head) {
        hashtable[index]->head = current->next;
        free(current);
      
        hashtable[index]->count--;
        pthread_mutex_unlock(&(hashtable[index]->lock));
        // delete is successful
        return 0; 
      }
      else {
        // Link previous node with one after current
        previous->next = current->next;
        free(current);
        hashtable[index]->count--;
        pthread_mutex_unlock(&(hashtable[index]->lock));
        // delete not successful
        return 1;
      }
    }
    previous = current;
    current = current->next;
  }

  pthread_mutex_unlock(&(hashtable[index]->lock));
  // value not found
  return 1;
}
// #.........Insert without lock.....#
void insert(int key,int data) {
   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->data = data;  
   item->key = key;
   int hashIndex = hashCode(key);
   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
      ++hashIndex;	
      hashIndex %= HASH_SIZE;
   }	
   hashArray[hashIndex] = item;
}
// #.........Delete without lock.....#
struct DataItem* delete(struct DataItem* item) {
   int key = item->key;
   int hashIndex = hashCode(key);
   while(hashArray[hashIndex] != NULL) {
      if(hashArray[hashIndex]->key == key) {
         struct DataItem* temp = hashArray[hashIndex]; 
         hashArray[hashIndex] = dummyItem; 
         return temp;
      }	
      ++hashIndex;
      hashIndex %= HASH_SIZE;
   }      	
   return NULL;        
}

struct DataItem *search(int key) {
   int hashIndex = hashCode(key);   
   while(hashArray[hashIndex] != NULL) {
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
      ++hashIndex;
      hashIndex %= HASH_SIZE;
   }       
   return NULL;        
}


int main() {
  list_l *list_t;
	//initialize the list
	List_Init(&list_t);
  char * a= "100";
  char * b="200";
  char * c= "300";
  printf("%s\n",a);

  //insert
	List_Insert(a);
  List_Insert(b);
  List_Insert(c);
  //delete-> return 0 if delete is successful and 1 if not successful 
  printf("%d\n",List_Delete(a));
  //return the found value
  printf("%s\n",List_Lookup(b));

// #......without locks......#
   dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
   dummyItem->data = -1;  
   dummyItem->key = -1; 
  // Insert
   insert(1, 20);
   insert(2, 70);

  //Delete
  item = search(1);
  delete(item);


//    pthread_t p1, p2;

//     pthread_create(&p1, NULL, test, list_t);
//     pthread_create(&p2, NULL, test, list_t);

//     pthread_join(p1, NULL);
//     pthread_join(p2, NULL);
	
}




