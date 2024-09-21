//
// Created by tgk on 9/20/24.
//

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

# define W_1 20// width of matrix 1 and height of matrix 2
# define W_2 12 // width of matrix 2 and resulting matrix
# define HEIGHT 10 // Height of matrix 1 and resulting matrix

// size of a matrix = width * height
# define SIZE(w,h) (w*h)
// Row of matrix
# define ROW(index,width) ( index / width)
// column of matrix
# define COLUMN(index, width) (index % width)
// index of array
# define INDEX(row, column, width) ((row * width) + column)

// It seems we have to declare a struct that can contain our arguments to pass to thread
struct params {
    int index; // we need index of array for our resulting array
    int* m1;  // first matrix
    int* m2;  // second matrix
    int *res; // resulting matrix
};

// this calculates a cell in a matrix
// what do we need?
// all params will be passed to struct and to thread.
void* calculateCell(void *args)
{
    // struct that holds our arguments
    // @see above
    struct params *p  = args;
    int index = p->index;
    int* m1 = p->m1;
    int* m2 = p->m2;
    int *res = p->res;

    // row and column of result matrix
    int col = COLUMN(index,W_2);
    int row = ROW(index, W_2);
    //printf("index is %d\n",row);

    // starting index on column 0 of first matrix, we get the row from row of resulting matrix
    int i_1 = INDEX(row,0, W_1);

    // starting index of second matrix at row 0, we get column of resulting matrix
    int i_2 = INDEX(0,col,W_2);

    // value of current cell = index of resulting matrix
    int cell = 0;

    // here we add product of matrix 1 columns and matrix 2 rows
    for(int i = 0; i < W_1; i++){
        // add cell of m1 * m2 and add it to cell value
        cell+= (m1[i_1] * m2[i_2]);
        // slide one column in matrix 1;
        i_1++;
        // and we slide down the column by one row
        i_2+= W_2;
    }
    // and we set value
    res[index] = cell;
    // struct is created using malloc, so we free it.
    free(p);
    return (NULL);
}
// finds a minumum or maximum value in a matrix
int find(int* matrix, int flag){

    // size of matrix;
    int size = W_2 * HEIGHT;
    // min or max is assumed as the first element of array.
    int value = matrix[0];
    for(int i = 0; i < size; i++){
        if(flag){ // we are looking for min
            if (matrix[i] < value){
                value = matrix[i];
            }
        }else{ // we looking for max
            if (matrix[i] > value){
                value = matrix[i];
            }
        }
    }
    return value;
}

// adds all items of matrix
int add(int* matrix){
    int sum = 0;
    for(int i = 0; i < W_2 * HEIGHT; i++){

        sum+= matrix[i];
    }
    return sum;
}

// Prints a matrix based on its width in a tabular mode
void printMatrix(int* matrix,int size, int width)
{
    // first cell
    printf("%d \t",matrix[0]);
    // the rest of the cells
    for(int i = 1; i < size; i++){
        // step down if column end
        if(i % width == 0){
            printf("\n");
        }
        printf("%d \t",matrix[i]);
    }
    printf("\n");
}

// parameter to thread to find max or min
struct matrix {
    int flag; // 1 will mean we want min, 0 max
    int value; // value to be returned -> min or max
    int* mat; // the matrix
};

// I am your father.
int main()
{
    // size of first array
    int size_1 = SIZE(W_1,HEIGHT);

    // first matrix
    int *m_1 = malloc(size_1 * sizeof(int);

    // setting random values for matrix 1
    for(int i = 0; i < size_1; i++){
        m_1[i] = random()%20;
    }

    // print first matrix
    printf("\nFirst matrix\n-------------------\n");
    printMatrix(m_1,size_1, W_1);

    // size of second matrix
    int size_2 = SIZE(W_1,W_2);

    // second matrix
    int *m_2 = malloc(size_2 * sizeof(int);

    // filling second matrix with random values
    for(int i = 0; i < size_2; i++){
        m_2[i] = random()%20;
    }

    // print second matrix
    printf("\nSecond matrix\n----------------------\n");
    printMatrix(m_2,size_2 , W_2);

    // resulting matrix size and innitialisation
    int res_size = HEIGHT * W_2;
    int *res = malloc(res_size * sizeof(int);
    //int *res = malloc(res_size * sizeof(int));

    // since every cell in the resulting matrix has to run in a separate thread,
    // we need to start as much threads as size of resulting matrix. we can do it in a loop may be
    // and to help us during Joining, we keep count of created threads
    int threadCount = 0;

    // creating our threads, to store thread ID, init array of size = number of threads we want
    pthread_t threads[res_size];

    // lets create the threads
    for(int i = 0; i < res_size; i++){

        // struct to hold our parameters - @ see struct above
        struct params *pr = malloc(sizeof(struct params));
        pr->index = i;
        pr->m1 = m_1;
        pr->m2 = m_2;
        pr->res = res;
        // we need to make sure a thread is created.
        if(!pthread_create(&threads[i], NULL, calculateCell,pr)){

            threadCount++;
        }else{
            printf("Thread %d creating failed\n",i);
        }
    }

    // lets join threads if the finished their task
    for(int i = 0; i < threadCount; i++){
        if(pthread_join(threads[i], NULL)){
            printf("Thread did not join");
        }
    }
    // we free first and second matrices as we are done with them
    free(m_1);
    free(m_2);

    printf("\n%d threads done\n", threadCount);
    printf("\nResulting matrix\n-----------------------\n");


    printMatrix(res,res_size , W_2);

    // and we print values
    printf("\nMinimaler Wert: %d; Maximaler Wert: %d; Summe aller Werte: %d\n",find(res,1),find(res,0),add(res));
// finally we free resulting matrix
free(res);

return NULL;




}