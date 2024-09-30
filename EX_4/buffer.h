//
// Created by tgk on 9/29/24.
//

#ifndef BUFFER_H
#define BUFFER_H

typedef struct CircularBuffer{
    int w_index;
    int r_index;
    int size;
    int *container;
    int is_full;
}CircularBuffer;


int dequeue(CircularBuffer *buffer, int *value);

int enqueue(CircularBuffer *buffer, int value);

int isEmpty(const CircularBuffer *buffer);

int isFull(const CircularBuffer *buffer);

CircularBuffer *init(int size);

void free_buffer(CircularBuffer *buffer);

#endif //BUFFER_H
