#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


//----------Counter with locks ----------------------//
 typedef struct __counter_t {
 	int value;
 	pthread_mutex_t lock;
 } counter_t;

 //static struct counter_t counter;

 void init(counter_t *c) {
 	c->value = 0;
 	pthread_mutex_init(&c->lock, NULL);
 }

 void increment(counter_t *c) {
 	pthread_mutex_lock(&c->lock);
 	c->value++;
 	pthread_mutex_unlock(&c->lock);
 }

 void decrement(counter_t *c) {
 	pthread_mutex_lock(&c->lock);
 	c->value--;
 	pthread_mutex_unlock(&c->lock);
 }

 int get(counter_t *c) {
 	pthread_mutex_lock(&c->lock);
 	int rc = c->value;
 	pthread_mutex_unlock(&c->lock);
 	return rc;
}

//----------------------end-----------------------------//

 //-----------------Counters Without Locks---------------//

void init2(counter_t *c) {
	c->value = 0;
}

void increment2(counter_t *c) {
	c->value++;
}

void decrement2(counter_t *c) {
	c->value--;
}
int get2(counter_t *c) {
	return c->value;
}



//-----------------------end---------------------------//



 void *mythread(void *arg)
 {
	 counter_t *counter = (counter_t *) arg;
	 int i;
	 for (i = 0; i < 10000000; i++) {
	    increment(counter);
	 }
	 return NULL;
}

void *mythread2(void *arg)
 {
	 counter_t *counter = (counter_t *) arg;
	 int i;
	 for (i = 0; i < 10000000; i++) {
	    increment2(counter);
	 }
	 return NULL;
}



 int main(int argc, char *argv[])
 {

counter_t *counter;
 pthread_t p1, p2;
 printf("------Counter with locks------\n");
 init(counter);
 printf("Begin (counter = %d)\n", get(counter));
 pthread_create(&p1, NULL, mythread, counter);
 pthread_create(&p2, NULL, mythread, counter);
// join waits for the threads to finish
 pthread_join(p1, NULL);
 pthread_join(p2, NULL);
 printf("Done with both threads (counter = %d)\n", get(counter));


 //counter_t *counter2;
 pthread_t p3, p4;
 printf("-----counter without locks------\n");
 init2(counter);
 printf("Begin (counter = %d)\n", get2(counter));
 pthread_create(&p3, NULL, mythread2, counter);
 pthread_create(&p4, NULL, mythread2, counter);
// join waits for the threads to finish
 pthread_join(p3, NULL);
 pthread_join(p4, NULL);
 printf("Done with both threads (counter = %d)\n", get2(counter));

 return 0;
}


