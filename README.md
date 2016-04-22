# parallel_product_matrix_matrix
A C implementation of a distributed algorithm for parallel product matrix matrix computation. Uses MPICH2 library

A lot of mathematical problems can be reduced to the solving of a big linear system. It's the case for PDE solving for instance. Often, you have to perform the basic operation of multiplying matrices.
When the size of the matrices to be multiplied gets very large, it becomes interesting to compute the product in parallel. 

Very big matrices are represented as 4D arrays (block matrices). This allows to localize the data, so that each processor can quickly gain access to the data it needs. A big matrix (M(i,j)) is then cut into p² submatrices, where p² is the number of processors. So we can spot each submatrix by its index I,J and we have : 
M(i,j,I,J) = M((I-1)n/p+i,(J-1)n/p+j) where n is the number of line of the matrix (= its number of columns)

Note : it only works with square matrices and they are generated randomly. Besides, the size of the matrix has to be divisible par p², where p² is the number of processors that will compute the product


To run the code : 
mpiexec –localonly [number_of_processors] matrix_matrix_product.exe [dimension]
Note : dimension should be equals to a perfect square (4,9,16,25,36...)
