#include "fonctions.h"


void multiply(double* A,double* B,int dim,double* result){
    int i;
    int j;
    int k;
    for(i=0;i<dim;i++){
        for(j=0;j<dim;j++){
            result[i*dim + j] = 0;
            for(k=0;k<dim;k++){
                result[i*dim + j] += A[i*dim + k] * B[k*dim + j];
            }
        }
    }
}

void add(double* A,double* B,int dim,double* result){
    int i=0;
    for (i=0;i<dim*dim;i++){
        result[i] = A[i]+B[i];
    }
}

double random(int min,int max){
    double r = (double)rand()/RAND_MAX;
    return min + r*(max-min);
}

void print_matrix(double *matrix, int number_of_rows, int number_of_columns){
    int i;
    int j;
    for(i=0;i<number_of_rows;i++){
        for(j=0;j<number_of_columns;j++){
            printf("%f ", matrix[i*number_of_columns + j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    printf("\n\n");
    fflush(stdout);
}

void fill(double *vector_to_be_filled, double *filling_vector, int size){
    int i;
    for (i=0;i<size;i++){
        vector_to_be_filled[i] = filling_vector[i];
    }
}
