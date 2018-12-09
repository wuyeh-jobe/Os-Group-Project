#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define BUCKETS (101)
// basic node structure
typedef struct __node_t{
	int key;
	struct __node_t * next;
} node_t;

// linked list structure
typedef struct __list_t{
	node_t * head;
	pthread_mutex_t lock;
    int len;
} list_t;

 typedef struct __hash_t {
    int len;
    pthread_mutex_t len_lock;
    list_t lists[BUCKETS];
 } hash_t;

//initialize list 
void List_Init(list_t *L){
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}

 void Hash_Init(hash_t *HT) {
    for (int i = 0; i < BUCKETS; i++) {
        List_Init(&HT->lists[i]);
    }
 }
//look up
int List_Lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr) {
        if (curr->key == key) {
            printf("%d",curr->key);
            printf("\n");
            rv = 0;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv; // now both success and failure
}

//insert with locks
int List_Insert(list_t *L, int key) {
    pthread_mutex_lock(&L->lock);
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("malloc");
        pthread_mutex_unlock(&L->lock);
        return -1; // fail
    }

    new_node->key  = key;
    new_node->next = L->head;
    L->head   = new_node;

    L->len++;
    pthread_mutex_unlock(&L->lock);

    return 0; // success
}
int Hash_Insert(hash_t *HT, int key) {
    pthread_mutex_lock(&HT->len_lock);
    int bucket = key % BUCKETS;
    HT->len++;
    pthread_mutex_unlock(&HT->len_lock);
    return List_Insert(&HT->lists[bucket], key);
}

//insert without locks
int Insert_WLock(list_t *L, int key){
    node_t * new = malloc(sizeof(node_t));
	if(new == NULL){
		perror("malloc");
		return -1; //fail
	}
	new ->key = key;
	new->next = L->head;
    L->head = new;
    L->len++;
    return 0; // success
}
int Hash_InsertWLock(hash_t *HT, int key) {
    int bucket = key % BUCKETS;
    HT->len++;
    return Insert_WLock(&HT->lists[bucket], key);
}
//delete with lock
int List_Delete(list_t *L, int key){
    node_t *curr = L->head;
    node_t *prev = NULL;
    pthread_mutex_lock(&L->lock);
    while(curr->key !=key && curr->next !=NULL){
        prev=curr;
        curr=curr->next;
    }
    if (curr->key == key){
        if (prev){
            prev->next = curr->next;
        }
        else {
            L->head = curr->next;
        }
        pthread_mutex_unlock(&L->lock);
        free (curr);
        return key;
    }
    pthread_mutex_unlock(&L->lock);
    return -1;
}
int Hash_Delete(hash_t *HT, int key){
    pthread_mutex_lock(&HT->len_lock);
    int bucket = key % BUCKETS;
    HT->len++;
    pthread_mutex_unlock(&HT->len_lock);
    return List_Delete(&HT->lists[bucket], key);
}

//determine hash length
int Hash_Length(hash_t *HT) {
    pthread_mutex_lock(&HT->len_lock);
    int len = HT->len;
    pthread_mutex_unlock(&HT->len_lock);
    return len;
}
//insert 2000 items into hash table
hash_t hash_table;
void * test_hasht(){
    for (int i = 0; i < 2000; i++) {
        Hash_Insert(&hash_table, i);
    }

    return NULL;
}
// test insert without locks
void * test_wlock(){
    for (int i = 0; i < 2000; i++) {
        Hash_InsertWLock(&hash_table, i*10);
    }
    return NULL;
}

// void * test_hasht_delete(){
//     for (int i = 0; i < 2000; ++i) {
//         Hash_Delete(&hash_table, i);
//     }

//     return NULL;
// }

int main(int argc, char *argv[]){
   
// Initiale Hash Table
    Hash_Init(&hash_table);
    hash_t *hash_tb;
    printf("Testing Insertion-100 Items\n");
    for (int i = 0; i < 100; i++) {
        Hash_Insert(&hash_table, i);
    }
    printf("Hash Table contains (No. of items): %d\n", Hash_Length(&hash_table));

    printf("With Locks\n");
    printf("Testing insertion with 2 threads:Inserting 2000 items (1000 in each)\n");
    pthread_t p1, p2;
    pthread_create(&p1, NULL, test_hasht, hash_tb);
    pthread_create(&p2, NULL, test_hasht, hash_tb);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("Hash Table contains (No. of items): %d\n", Hash_Length(&hash_table));


    printf("Without Locks\n");
    printf("Testing insertion with 2 threads:Inserting 2000 items (1000 in each)\n");
    pthread_t p3, p4;
    pthread_create(&p3, NULL, test_wlock, hash_tb);
    pthread_create(&p4, NULL, test_wlock, hash_tb);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    printf("Hash Table contains (No. of items): %d\n", Hash_Length(&hash_table));
    return 0;
}