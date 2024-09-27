
#include <pthread.h>


/**
 * Type of the elements of the circular buffer
 * If you want to use a different element type, then change this typedef
 */
typedef int cb_element_t;


/**
 * Struct representing a circular buffer
 *
 * There are two possible ways to protect the critical regions of the circular buffer:
 *
 * - Use a single global mutex to protect the critical regions
 *     - Advantage: Easy to implement
 *     - Disadvantage: Lost performance due to unnecessary waiting. When e.g. cb_add() is
 *       called at the same time for two independent circular buffer, only one can access the
 *       critical regions. But there is no problem for both accessing the critical region at the
 *       same time since they are independent.
 *
 * - Use a separate mutex for each circular buffer:
 *     - Advantage: Better performance since independent circular buffers can access the critical regions
 *       at the same time
 *     - Disadvantage: More effort required when implementing
 *
 * Here we are using a separate mutex for each circular buffer. Therefore we need to extend this data structure
 * and add a mutex and two condition variables. The condition variables are used to wake up sleeping consumers or
 * producers.
 */
typedef struct {
	cb_element_t* buffer;
	int first;  // Index des ersten Elements
	int count;  // Anzahl der Elemente im Buffer
	int size;   // Größe des Buffers
	pthread_mutex_t mutex;  // Protecting mutex
	pthread_cond_t signalConsumer;  // Condition to wake up a sleeping consumer
	pthread_cond_t signalProducer;  // Condition to wake up a sleeping producer
	int finish_flag; // Flag signaling that we want to finish the program
} circularbuffer_t;


/**
 * Initializes the circular buffer.
 *
 * \param buffer pointer to struct representing a circular buffer
 * \param size size of the circular buffer
 * \return a negative value in case of an error otherwise 0
 */
int cb_init(circularbuffer_t* buffer, int size);


/**
 * Destroys the circular buffer and frees all ressources.
 *
 * \param buffer pointer to struct representing the circular buffer
 * \return a negative value in case of an error otherwise 0
 */
int cb_destroy(circularbuffer_t* buffer);


/**
 * Adds an Element to the circular buffer
 *
 * Blocks when the buffer is full until at least one element has been removed.
 *
 * \param buffer pointer to struct representing a circular buffer
 * \param elem value to be added to the circular buffer
 */
void cb_add(circularbuffer_t* buffer, cb_element_t elem, int id);


/**
 * Removes an Element from the circular buffer.
 *
 * Blocks when the buffer is empty until at least one element has been added.
 *
 * \param buffer pointer to struct representing a circular buffer
 * \param elem pointer to where the removed value should be stored (call-by-reference)
 * \return removed element
 */
int cb_remove(circularbuffer_t* buffer);


/**
 * Returns the number of elements in the circular buffer
 *
 * \param buffer pointer to struct representing a circular buffer
 * \return number of elements in the circular buffer
 */
int cb_count(circularbuffer_t* buffer);


/**
 * Returns whether the circular buffer is full
 *
 * \param buffer pointer to struct representing a circular buffer
 * \return boolean value indicating that the buffer is full
 */
int cb_isFull(circularbuffer_t* buffer);


/**
 * Returns whether the circular buffer is empty
 *
 * \param buffer pointer to struct representing a circular buffer
 * \return boolean value indicating whether the buffer is empty
 */
int cb_isEmpty(circularbuffer_t* buffer);


/**
 * Wakes up all sleeping producers and consumers and let them finish their respective cb_add() or cb_remove() calls.
 *
 * When we want to gracefully shut down our program we need a way to wake up all sleeping threads and tell them that
 * they should finish their calls to cb_add() or cb_remove(). This is done by setting an appropriate flag.
 *
 * \param buffer pointer to struct representing a circular buffer
 */
void cb_finalize(circularbuffer_t* buffer);




 
