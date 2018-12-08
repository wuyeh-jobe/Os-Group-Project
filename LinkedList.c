#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include<pthread.h>

//BASIC NODE STRUCTURE
typedef struct __node_t{
	int key;
	struct __node_t * next;
} node_t;


//BASCIC LIST STRUCTURE
typedef struct __list_t{
	node_t * head;
	pthread_mutex_t lock;
} list_t;


//INITIALIZE THE LIST
void List_Init(list_t *L){
	L->head = NULL;
	pthread_mutex_init(&L->lock, NULL);
}


//INSERT WITH LOCKS
void List_Insert(list_t *L, int key){
	//create a node
	node_t * new = malloc(sizeof(node_t));
	if(new == NULL){
		perror("malloc");
		return;
	}
	//set the key of the node
	new ->key = key;

	//lock critical section
	pthread_mutex_lock(&L->lock);
	//set the next node
	new->next = L->head;
	//set the new head
	L->head = new;
	pthread_mutex_unlock(&L->lock);
}


//INSET WITHOUT LOCKS
void List_Insert2(list_t *L, int key){
	//create a node
	node_t * new = malloc(sizeof(node_t));
	if(new == NULL){
		perror("malloc");
		return;
	}

	//set the key of the node
	new ->key = key;
	//set the next node
	new->next = L->head;
	//set the new head
	L->head = new;
}


//DELETE WITH LOCKS
int List_Delete(list_t *L, int key){
	int c =-1;
	pthread_mutex_lock(&L->lock);
	//store head node
	node_t* cur = L->head, *prev; 

	 //if head node holds the key to be deleted
	 if(cur !=NULL && cur->key == key){
	 	//set head head.next
	 	L->head = cur->next;
	 	free(cur);
	 	c =0;
	 }
	 else{
	 //search for the key to be deleted
	 	while(cur != NULL && cur->key!=key){
	 		prev = cur;
	 		cur = cur->next;
	 	}

	 	//key not found
	 	if(cur == NULL) {
	 		c = -1;
	 	}
	 	else{
	 	//else set prev.next to temp.next
	 		prev->next = cur->next;
			c = 0;
			free(cur);
		}
	}
	pthread_mutex_unlock(&L->lock);
	return c;
}


//DELETE WITHOUT LOCKS
int List_Delete2(list_t *L, int key){
	int c =-1;
	//store head node
	node_t* cur = L->head, *prev; 

	 //if head node holds the key to be deleted
	 if(cur !=NULL && cur->key == key){
	 	//set head head.next
	 	L->head = cur->next;
	 	free(cur);
	 	c =0;
	 }
	 else{
	 //search for the key to be deleted
	 	while(cur != NULL && cur->key!=key){
	 		prev = cur;
	 		cur = cur->next;
	 	}

	 	//key not found
	 	if(cur == NULL) {
	 		c = -1;
	 	}
	 	else{
	 	//else set prev.next to temp.next
	 		prev->next = cur->next;
			c = 0;
			free(cur);
		}
	}
	return c;

}


//LIST LOOKUP AT A PARTICULAR KEY WITH A LOCK
int List_Lookup(list_t * L, int key){
	int rv = -1;
	pthread_mutex_lock(&L->lock);
	//set curr to the head
	node_t * curr = L->head;
	//while curr exists
	while(curr){
		if(curr->key == key){
			rv = 0;
			break;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&L->lock);
	return rv;
}


//LIST LOOKUP AT A PARTICULAR KEY WITHOUT A LOCK
int List_Lookup2(list_t * L, int key){
	int rv = -1;
	//set curr to the head
	node_t * curr = L->head;
	//while curr exists
	while(curr){
		if(curr->key == key){
			rv = 0;
			break;
		}
		curr = curr->next;
	}
	return rv;
}

void *test1(void *arg)
 {

	 list_t *list = (list_t *) arg;
	 int i;
	 for (i = 0; i < 10000; i++) {
	 	List_Insert(list, i);
	 	List_Delete(list, i);	    
	 }
	 printf("%s", " Last Item Inserted and deleted successfully ");
	 printf("%d\n", i);

	 return NULL;
}

void *test2(void *arg)
 {
	 list_t *list = (list_t *) arg;
	 int i;
	 for (i = 0; i < 10000; i++) {
	    List_Insert2(list, i);
	 	List_Delete2(list, i);	    
	 }
}

int main(){
	list_t *list;
	//initialise the list
	List_Init(list);

	int testValue = 0; // Change this value to test the second scenario

	pthread_t p1, p2, p3, p4;

		printf("%s\n", "------------------With Locks------------------------");
		printf("Testing Insert and Delete with locks\n");
		pthread_create(&p1, NULL, test1, list);
		pthread_create(&p2, NULL, test1, list);
		// join waits for the threads to finish
		pthread_join(p1, NULL);
		pthread_join(p2, NULL);
		printf("Ended successfully!!\n\n");


		printf("%s\n", "------------------Without Locks------------------------");
		printf("Testing Insert and Delete without locks\n");
		printf("A segmentation fault error may occur at any time due to \n");
		printf("%s\n", "a NULL pointer error caused by an interrupt");
		pthread_create(&p3, NULL, test2, list);
		pthread_create(&p4, NULL, test2, list);
		// join waits for the threads to finish
		pthread_join(p3, NULL);
		pthread_join(p4, NULL);
		printf("%s\n", "This may not be printed!!");
	
	
}