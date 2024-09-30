//
// Created by tgk on 9/29/24.
// remeber to call free_buffer(buffer) if you use this data structure.
//

# include "buffer.h"
# include <stdio.h>
# include <stdlib.h>

// read value and store it to pointer
// return success or failure 0 = error, 1 = success
int dequeue(CircularBuffer *buffer, int *value)
{
    if(isEmpty(buffer)){
        //fprintf(stderr, "READ ERROR : Buffer is full\n");
        return 0;
    }

    // if buffer was full before we started removing items
    // set it to false. because we removed one.
    if(buffer->is_full) {
        buffer->is_full = 0;
    }

    // set value to our access pointer
    *value = buffer->container[buffer->r_index];
    // and update our read index
    buffer->r_index = (buffer->r_index+1)%buffer->size;
    return 1;
}

int enqueue(CircularBuffer *buffer, const int value)
{
    if(buffer->is_full){
        //fprintf(stderr, "WRITE ERROR : Buffer is full\n");
        return 0;
    }
    // if buffer is filled and the 'is_full' flag isnt set already
    // we set it here.
    if((buffer->w_index+1) % buffer->size == buffer->r_index) {
        buffer->is_full = 1;
    }
    buffer->container[buffer->w_index] = value;
    //printf("Wrote %d to buffer\n",value);
    buffer->w_index = (buffer->w_index +1) % buffer->size;
    return 1;
}

int isEmpty(const CircularBuffer *buffer)
{
    return buffer->w_index == buffer->r_index && buffer->is_full == 0;
}

// init buffer. we allocate memory to buffer and its container.
CircularBuffer *init(const int size)
{
    // allocate memory to buffer and null check
    CircularBuffer *buffer = malloc(sizeof(CircularBuffer));
    if(buffer == NULL) {
        fprintf(stderr, "Error alocating memory to buffer\n");
        return  NULL;
    }
    // allocate memory to buffer container and null check
    buffer->container = malloc(sizeof(int) * size);
    if(buffer->container == NULL) {
        fprintf(stderr, "Error alocating memory to buffer container\n");
        free(buffer);
        return  NULL;
    }
    // and set the default values for all members
    buffer->size = size;
    buffer->r_index = 0;
    buffer->w_index = 0;
    buffer->is_full = 0;
    return buffer;
}

// we free buffer and its container
void free_buffer(CircularBuffer *buffer) {
    // here both buffer and the buffer container should be freed
    free(buffer->container);
    free(buffer);
}
