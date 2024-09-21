#include <pthread.h>
#include <stdio.h>

/* Added by Tinsae for Ex_3 */
// create a mutex lock

pthread_mutex_t lock;

/* end of added code*/

void* thread_func(void* arg) {

    /* Added by Tinsae for Ex_3 */

    // lock mutex because thread is using a shared data (myData) here
    pthread_mutex_lock(&lock);
    /* end of added code */

	int* mydata = (int*)arg;
	// Wir addieren zur übergebenen Zahl 10000-mal die 1
	for (int i = 0; i < 10000; i++) {
		*mydata += 1;
	}
    
    /* Added by Tinsae for Ex_3 */
    // since thread has finished working on shared data, mutex can be unlocked
    pthread_mutex_unlock(&lock);
    /* end of added code*/

	return NULL;
}

int main() {
    /* added by Tinsae for ex_3*/
    // first we initialise mutex
    if (pthread_mutex_init(&lock, NULL)){ // Mutex init should return 0 on success
        printf("\n mutex init failed\n");
        return 1;
    }

	// Reserviere Speicher für die Thread-Handles
	pthread_t myThreads[10];
	// Reserviere Speicher für die benutzerdefinierten Daten
	// Zu dieser Zahl werden 10 Threads 10000-mal die 1 dazuaddieren
	int mydata = 0;

	// Erzeuge 10 Threads
	for (int i = 0; i < 10; i++) {
		printf("Erzeuge Thread %d\n", i);
		// Erzeuge den Thread
		pthread_create(&myThreads[i], NULL, &thread_func, &mydata);
	}

	// Warte auf die Beendigung aller Threads
	for (int i = 0; i < 10; i++) {
		pthread_join(myThreads[i], NULL);
		printf("Thread %d wurde beendet\n", i);
	}

	// Gebe das Endergebnis aus
	// Wenn 10 Threads 10000-mal die 1 dazuaddieren, müsste doch 100000 rauskommen
	// Tut es das?
	// Wenn es das tut, ist das immer der Fall?
	printf("mydata = %d\n", mydata);

    /* added by Tinsae for ex_3*/
    // destroy mutex 
    pthread_mutex_destroy(&lock);
    /* end of added code */

	return 0;
}
