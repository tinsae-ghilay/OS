
#include "circularbuffer.h"
#include <stdio.h>
#include <stdlib.h>



int cb_init(circularbuffer_t* buffer, int size) {
	if (size <= 0) {
		return -1;
	}
	buffer->buffer = (cb_element_t*)malloc(sizeof(cb_element_t)*size);
	if (buffer->buffer == NULL) {
		return -2;
	}
	buffer->first = 0;
	buffer->count = 0;
	buffer->size = size;
	buffer->finish_flag = 0;
	// Initialize mutex and condition variables
	pthread_mutex_init(&buffer->mutex, NULL);
	pthread_cond_init(&buffer->signalConsumer, NULL);
	pthread_cond_init(&buffer->signalProducer, NULL);
	return 0;
}


int cb_destroy(circularbuffer_t* buffer) {
    if (buffer->buffer) {
        free(buffer->buffer);
        buffer->buffer = NULL;
    	// Destroy mutex and condition variables
    	pthread_mutex_destroy(&buffer->mutex);
    	pthread_cond_destroy(&buffer->signalConsumer);
    	pthread_cond_destroy(&buffer->signalProducer);
        return 0;
    }
    return -1;
}


void cb_add(circularbuffer_t* buffer, cb_element_t elem, int id) {
	// Adding a new element to the circular buffer is a critical region
	// since we need to modify the circular buffer.
	 pthread_mutex_lock(&buffer->mutex); // Lock mutex before the critical region

	 // When the circular buffer is full, let the producer wait till an element has been removed
	 // IMPORTANT: It is essential to re-check the the buffer capacity AFTER we have been woken up
	 // because when more than one producer has been woken up the buffer could already be full again.
	 // That's why we are using a while-loop here.
	 while (!buffer->finish_flag && buffer->count >= buffer->size) {
		 printf("Temperatursensor %d wurde schlafengelegt!\n", id);
		 pthread_cond_wait(&buffer->signalProducer, &buffer->mutex);
		 printf("Temperatursensor %d wurde aufgeweckt!\n", id);
	 }

	 // Start of critical region
	 if (!buffer->finish_flag)  {
		 int elem_index = (buffer->first + buffer->count) % buffer->size;
		 buffer->buffer[elem_index] = elem;
		 // When the buffer was empty before we added the new element
		 // then there COULD be some consumers waiting.
		 // That's why we call signal() on the consumer condition variable
		 // We don't need to actually check whether there is really a consumer waiting
		 if (buffer->count <= 0) {
			 pthread_cond_signal(&buffer->signalConsumer);
		 }
		 buffer->count++;
	 }
		 // End of critical region

	 // Unlock mutex after critical region
	 pthread_mutex_unlock(&buffer->mutex);
}


int cb_remove(circularbuffer_t* buffer) {
	cb_element_t elem;

	// Removing an element from the circular buffer is a critical region
	// since we need to modify the circular buffer.
	 pthread_mutex_lock(&buffer->mutex); // Lock mutex before the critical region

	 // When the circular buffer is empty, let the consumer wait till an element has been added
	 // IMPORTANT: It is essential to re-check the the buffer capacity AFTER we have been woken up
	 // because when more than one consumer has been woken up the buffer could already be empty again.
	 // That's why we are using a while-loop here.
	 while (!buffer->finish_flag && buffer->count <= 0) {
		 printf("Logging Programm wurde schlafengelegt!\n");
		 pthread_cond_wait(&buffer->signalConsumer, &buffer->mutex);
		 printf("Logging Programm wurde aufgeweckt!\n");
	 }

	 // Start of critical region

	 // Start of critical region
	 if (!buffer->finish_flag)  {
		 elem = buffer->buffer[buffer->first];
		 buffer->first = (buffer->first + 1) % buffer->size;
		 // When the buffer was full before we removed an element
		 // then there COULD be some producers waiting.
		 // That's why we call signal() on the producer condition variable
		 // We don't need to actually check whether there is really a producer waiting
		 if (buffer->count >= buffer->size) {
			 pthread_cond_signal(&buffer->signalProducer);
		 }
		 buffer->count--;
	 }
	 // End of critical region

	 // Unlock mutex after critical region
	 pthread_mutex_unlock(&buffer->mutex);
	 return elem;
}


int cb_count(circularbuffer_t* buffer) {
	 return buffer->count;
}


int cb_isFull(circularbuffer_t* buffer) {
	 return buffer->count == buffer->size;
}


int cb_isEmpty(circularbuffer_t* buffer) {
	 return buffer->count == 0;
}



void cb_finalize(circularbuffer_t* buffer) {
	pthread_mutex_lock(&buffer->mutex);
	buffer->finish_flag = 1;
	pthread_cond_broadcast(&buffer->signalConsumer);
	pthread_cond_broadcast(&buffer->signalProducer);
	pthread_mutex_unlock(&buffer->mutex);
}

