#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>


typedef struct __node_t {
	int	value;
	struct __node_t	*next;
} node_t;

typedef struct __queue_t {
	node_t	*head;
	node_t	*tail;
	pthread_mutex_t	headLock;
	pthread_mutex_t	tailLock;
	pthread_mutex_t	iLock;
} queue_t;

void Queue_Init(queue_t *q) {
	node_t *tmp = malloc(sizeof(node_t));
	tmp->next = NULL;
	q->head = q->tail = tmp;
	pthread_mutex_init(&q->headLock, NULL);
	pthread_mutex_init(&q->tailLock, NULL);
	pthread_mutex_init(&q->iLock, NULL);
}

//--------------Queue with locks ----------------//
void Queue_Enqueue(queue_t *q, int value) {
	node_t *tmp = malloc(sizeof(node_t));
	assert(tmp != NULL);
	tmp->value = value;
	//printf("%d\n", value);
	tmp->next = NULL;
	pthread_mutex_lock(&q->tailLock);
	q->tail->next = tmp;
	q->tail = tmp;
	pthread_mutex_unlock(&q->tailLock);

}

int Queue_Dequeue(queue_t *q, int *value) {
	pthread_mutex_lock(&q->headLock);
	node_t *tmp = q->head;
	node_t *newHead = tmp->next;
	if (newHead == NULL) {
		pthread_mutex_unlock(&q->headLock);
		return -1; // queue was empty
	}	
	*value = newHead->value;
	//printf("%d\n", *value);
	q->head = newHead;
	pthread_mutex_unlock(&q->headLock);
	free(tmp);
	return 0;
}

char* Queue_Delete(queue_t *q, int value){
	char* found = "";
	pthread_mutex_lock(&q->iLock);
	//store head node
	node_t* cur = q->head, *prev; 

	 //if head node holds the key to be deleted
	 if(cur !=NULL && cur->value== value){
	 	//set head head.next
	 	q->head = cur->next;
	 	free(cur);
	 	found = "Value deleted.";
	 }
	 else{
	 //search for the key to be deleted
	 	while(cur != NULL && cur->value!=value){
	 		prev = cur;
	 		cur = cur->next;
	 	}

	 	//value not found
	 	if(cur == NULL) {
	 		found = "Value not found.";
	 	}
	 	else{
	 	//else set prev.next to temp.next
	 		prev->next = cur->next;
			found = "Value deleted";
			free(cur);
		}
	}

	pthread_mutex_unlock(&q->iLock);
	return found;

}

char* Queue_Lookup(queue_t *q, int value){
	char* rv = "Value not in queue.";
	pthread_mutex_lock(&q->iLock);
	node_t * curr = q->head;
	while(curr){
		if(curr->value == value){
			rv = "Value in queue."; //Value Found
			break;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&q->iLock);
	return rv;
}


//---------------End of Queue with locks----------------//



//------------Queue without locks ----------------------//
void Queue_Enqueue2(queue_t *q, int value) {
	node_t *tmp = malloc(sizeof(node_t));
	assert(tmp != NULL);
	tmp->value = value;
	//printf("%d\n", value);
	tmp->next = NULL;
	q->tail->next = tmp;
	q->tail = tmp;

}

int Queue_Dequeue2(queue_t *q, int *value) {
	node_t *tmp = q->head;
	node_t *newHead = tmp->next;
	if (newHead == NULL) {
		return -1; // queue was empty
	}	
	*value = newHead->value;
	//printf("%d\n", *value);
	q->head = newHead;
	free(tmp);
	return 0;
}

//------------End of Queue without locks ------------//


void *test1(void *arg)
 {

	 queue_t *queue = (queue_t *) arg;
	 int i;
	 for (i = 0; i < 20000000; i++) {
	 	Queue_Enqueue(queue, i);
	 	Queue_Dequeue(queue, &i);	    
	 }
	 printf("%s", " Last Item Enqueued and Dequeued ");
	 printf("%d\n", i);

	 return NULL;
}

void *test2(void *arg)
 {
	 queue_t *queue = (queue_t *) arg;
	 int i;
	 for (i = 0; i < 20000000; i++) {
	    Queue_Enqueue2(queue, i);
	 	Queue_Dequeue2(queue, &i);	    
	 }
}

int main(int argc, char const *argv[])
{
	//Intantiate queue
	queue_t *queue;
	//Intialize Queue
	Queue_Init(queue);


	volatile int testValue = 0; // Change this value for the different tests

	pthread_t p1, p2;

	if (testValue == 0){
		printf("Testing Enqueue and Dequeue with locks (Two threads used).......\n");
		pthread_create(&p1, NULL, test1, queue);
		pthread_create(&p2, NULL, test1, queue);
		// join waits for the threads to finish
		pthread_join(p1, NULL);
		pthread_join(p2, NULL);
		printf("Ended successfully!!\n");
		printf("%s\n", "Change testValue to test without locks, and delete");



	}
	else if (testValue == 1){
		printf("Testing Enqueue and Dequeue without locks\n");
		printf("A segmentation fault may happen because of a possible\n");
		printf("%s\n", " NULL pointer error caused by an interrupt....");
		pthread_create(&p1, NULL, test2, queue);
		pthread_create(&p2, NULL, test2, queue);
		// join waits for the threads to finish
		pthread_join(p1, NULL);
		pthread_join(p2, NULL);
		printf("%s\n", "Oh it Worked :( Let's try again");
	}	
	else{
		//-----------Testing for delete --------//
		int value = 100;
		Queue_Enqueue(queue, value);
		printf("%s\n", Queue_Lookup(queue,value));
		printf("%s\n", Queue_Delete(queue,value));
		printf("%s\n", Queue_Lookup(queue,value));
		printf("%s\n", "-----------------------");
		//-------------------------------------//
	}

	
	return 0;
}