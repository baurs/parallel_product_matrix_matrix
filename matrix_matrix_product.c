#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include "matrix_matrix_functions.c"
#include <time.h>

int main(int argc, char *argv[]){
    srand(time(NULL));
    MPI_Init(&argc, &argv);
    int rank,size;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int P = (int)sqrt(size);
    int tag, source, dest;

    int dim = atoi(argv[1]); // dimensions of the square input matrix to be generated
    int subdim = dim/P;
    int I,J;
    double *A_local;
    double *B_local;
    double *C_local;
    double *A_ext;
    double *B_ext;
    double *C_tmp;
    A_local = malloc(sizeof(double)*subdim*subdim);
    B_local = malloc(sizeof(double)*subdim*subdim);
    C_local = malloc(sizeof(double)*subdim*subdim);
    A_ext = malloc(sizeof(double)*subdim*subdim);
    B_ext = malloc(sizeof(double)*subdim*subdim);
    C_tmp = malloc(sizeof(double)*subdim*subdim);
    double A[subdim][subdim][P][P];
    double B[subdim][subdim][P][P];
    double C[subdim][subdim][P][P];

    /* Communicators */
    I = rank%P;
    J=rank/P;
    MPI_Comm row_communicator;
    MPI_Comm column_communicator;
    MPI_Comm_split(MPI_COMM_WORLD, I, J, &row_communicator);
    MPI_Comm_split(MPI_COMM_WORLD, J, I, &column_communicator);

    int i,j,k,l;

    /* Fills C_local with 0s */
    for(i=0;i<subdim;i++){
        for(j=0;j<subdim;j++){
            C_local[i*subdim+j] = 0;
        }
    }


    /* 1) Fills A and B and sends local matrices to other processors */
    if (rank==0){
        /* 1.1) Fills randomly A and B */
        for(i=0;i<subdim;i++){
            for(j=0;j<subdim;j++){
                for(k=0;k<P;k++){
                    for(l=0;l<P;l++){
                        A[i][j][k][l] = random(-10,10);
                        B[i][j][k][l] = random(-10,10);
                    }
                }
            }
        }
        printf("Matrix A : \n"); fflush(stdout);
        for(k=0;k<P;k++){
            for(i=0; i<subdim;i++){
                printf("\n");
                fflush(stdout);
                for(l=0;l<P;l++){
                    for(j=0;j<subdim;j++){
                        printf("%f ",A[i][j][k][l]);
                        fflush(stdout);
                    }
                }
            }
        }
        printf("\n \n \n");

        printf("Matrix B : \n"); fflush(stdout);
        for(k=0;k<P;k++){
            for(i=0; i<subdim;i++){
                printf("\n");
                fflush(stdout);
                for(l=0;l<P;l++){
                    for(j=0;j<subdim;j++){
                        printf("%f ",B[i][j][k][l]);
                        fflush(stdout);
                    }
                }
            }
        }
        printf("\n \n \n");



        /* 1.2) Sends A_local and B_local to everyone */
        for(dest=1;dest<size;dest++){
            I = dest%P;
            J = dest/P;
            for (i=0;i<subdim;i++){
                for(j=0;j<subdim;j++){
                    A_local[i*subdim+j] =A[i][j][I][J];
                    B_local[i*subdim+j] =B[i][j][I][J];
                }
            }
            MPI_Send(A_local, subdim*subdim, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
            MPI_Send(B_local, subdim*subdim, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
        }
        /* Fills your own A_local and B_local */
        for (i=0;i<subdim;i++){
            for(j=0;j<subdim;j++){
                A_local[i*subdim+j] =A[i][j][0][0];
                B_local[i*subdim+j] =B[i][j][0][0];
            }
        }
    }


    /* 2) Receive the local matrices from 0 */
    else{
        source = 0;
        MPI_Recv(A_local, subdim*subdim, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(B_local, subdim*subdim, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    }


    /* 3) Every processor computes the products */
    fill(A_ext,A_local,subdim*subdim);
    fill(B_ext,B_local,subdim*subdim);
    for(k=0;k<P;k++){
        if(k==J){
            fill(A_ext, A_local,subdim*subdim);
        }
        if(k==I){
            fill(B_ext,B_local,subdim*subdim);
        }
        MPI_Bcast(A_ext,subdim*subdim,MPI_DOUBLE,k,row_communicator);
        MPI_Bcast(B_ext,subdim*subdim,MPI_DOUBLE,k,column_communicator);
        multiply(A_ext,B_ext,subdim,C_tmp);
        add(C_tmp, C_local, subdim, C_local);
    }

    /* 4) Printing the results */
    /* 4.1) Puts master's own results into C*/
    if(rank==0){
        for(i=0;i<subdim;i++){
            for(j=0;j<subdim;j++){
                C[i][j][0][0] = C_local[i*subdim+j];
            }
        }
    }


    /* 4.2) Every slave sends its local results to 0 */
    else{
        dest=0;
        MPI_Send(C_local, subdim*subdim, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
    }


    /* 4.3) The master receives all the results from the other processors and puts them into C */
    if(rank==0){
        for(source=1;source<size;source++){
            I = source%P;
            J = source/P;
            MPI_Recv(C_local, subdim*subdim, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
            for(i=0;i<subdim;i++){
                for(j=0;j<subdim;j++){
                    C[i][j][I][J] = C_local[i*subdim+j];
                }
            }
        }
        /* Prints the result */
        printf("Result : \n"); fflush(stdout);
        for(k=0;k<P;k++){
            for(i=0; i<subdim;i++){
                printf("\n");
                fflush(stdout);
                for(l=0;l<P;l++){
                    for(j=0;j<subdim;j++){
                        printf("%f ",C[i][j][k][l]);
                        fflush(stdout);
                    }
                }
            }
        }
        printf("\n \n \n");
    }


    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    free(C_local);
    free(C_tmp);
    free(A_local);
    free(B_local);
    free(B_ext);
    free(A_ext);



    return 0;
}
