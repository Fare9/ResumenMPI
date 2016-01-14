/* 
Integración de un sistema por el algoritmo de Gauss-Seidel.
Algoritmo:
    1. Se realiza una división por bloques. El número de procesos ha de ser un
    divisor de la constante MATRIZ.
    2. Cada proceso inicializa su bloque. A cada bloque se le añaden dos filas
    “fantasma” para intercambiar datos con los procesos asociados a los bloques
    superior e inferior.
    3a. Cada proceso realiza el cómputo del promedio, intercambiando los
    resultados con sus vecinos.
    3b. El proceso 0 “reduce” las diferencias de todos, para ver si se ha de
    terminar. En tal caso, difunde el valor done=0 a todos los demás.
    4. Cada proceso imprime su bloque en un fichero.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

# define MATRIZ     (128)
# define LAMBDA     0.2
# define LADO_I     30
# define LADO_D     30
# define ARRIBA     50 
# define ABAJO      50
# define INTERNO    15
# define FILA       1
# define TOL        0.001

void Resuelve(float myA[][MATRIZ+2], int pid,int nprocs) {
    /*
        Inicializamos valores para resolver la integral, segun el numero de procesos
        que tengamos y si el proceso es par o impar entonces recibimos y enviamos unos valores
        u otros
        Luego simplemente resolvemos la integral y llamamos a Reduce, entonces el proceso 0 pone la variable 
        de acabado a 1 y la envía a todo el mundo 
    */
    int i,j, n1 = 2 + MATRIZ, n2 = 2 + MATRIZ/nprocs, done = 0;
    float temp, diff, mydiff = 0;
    MPI_Status status;

    while (!done) {
        mydiff = 0;
        if (pid != 0)
            MPI_Send(&myA[1][0], n1, MPI_FLOAT, pid - 1,FILA, MPI_COMM_WORLD);
        if (pid != nprocs-1)
            MPI_Send(&myA[n2 - 2][0], n1, MPI_FLOAT, pid+ 1, FILA,MPI_COMM_WORLD);
        if (pid != 0)
            MPI_Recv(&myA[0][0], n1, MPI_FLOAT, pid - 1,FILA,MPI_COMM_WORLD, &status);
        if (pid != nprocs-1)
            MPI_Recv(&myA[n2 - 1][0], n1, MPI_FLOAT, pid+ 1, FILA,MPI_COMM_WORLD, &status);

        for (i = 1; i < n2 - 1; i++) {
            for (j = 1; j < n1 - 1; j++) {
                temp = myA[i][j];
                myA[i][j] = LAMBDA*(myA[i][j] + myA[i][j-1] + myA[i-1][j] + myA[i][j+1] + myA[i+1][j]);
                mydiff += fabs(myA[i][j] - temp);

            }
        }

        MPI_Reduce(&mydiff, &diff, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (pid == 0) {
            if (diff/(MATRIZ*MATRIZ) < TOL) {
                done = 1;
            }
        }

        MPI_Bcast(&done, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

}

void Inicializa(float myA[][MATRIZ+2], int pid, int nprocs) {
    /*
        Metodo para incializar las variables 
    */
    int i,j;
    int n1 = 2 + MATRIZ, n2 = 2 + MATRIZ/nprocs;

    for (i = 0; i < n2; i++){
        for (j = 0; j < n1; j++)
            myA[i][j] = INTERNO;
    }

    for (i = 0; i < n2; i++){
        myA[i][0]    =   LADO_I;
        myA[i][n1-1] =   LADO_D;
    }


    if (pid == 0) {
        for (j = 0; j < n1; j++) {
            myA[0][j] = ARRIBA;
        }
    }

    if (pid == nprocs - 1) {
        for (j = 0; j < n1; j++) {
            myA[n2 - 1][j] = ABAJO;
        }
    }
}

void PrintData(float myA[][MATRIZ+2], int pid, int nprocs){
    /*
        Metodo para crear un archivo con los resultados.
    */
    int i, j;
    int n1 = 2 + MATRIZ;
    int n2 = 2 + MATRIZ/nprocs;
    FILE *fpc = NULL;
    char name[256] = "";

    sprintf(name, "out%d.dat", pid);

    if ((fpc = fopen(name, "w")) != NULL) {
        for (i = 1; i < n2 - 1 ; i++){
            for (j = 1; j < n1 - 1; j++)
                fprintf(fpc, "%.3f ", myA[i][j]);
            fprintf(fpc, "\n");
        }
        fclose(fpc);
    }
}

in main(int argc, char *argv[]) {
    float *myA = NULL; /* Matriz en este proceso*/
    int pid = 0; /* Rango del proceso*/
    int nprocs = 0; /*Número de procesos usados */

    MPI_Init(&argc, &argv);
    /****************************************/
        MPI_Comm_rank(MPI_COMM_WORLD, &pid); //miro mi identificador
        MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //miro el numero de procesos
        myA = (float *)malloc(sizeof(float)*(2 + MATRIZ)*(2 + MATRIZ/nprocs)); //puntero para recorrer matriz
        Inicializa(myA, pid, nprocs); /* Inicializamos la matriz */
        MPI_Barrier(MPI_COMM_WORLD); /* Espera que se inicialicen todos */
        Resuelve(myA, pid, nprocs); /* Resolvemos */
        MPI_Barrier(MPI_COMM_WORLD); /* Espera que acaben todos */
        PrintData(myA, pid, nprocs); /* Imprimimos los resultados en ficheros */

    /*****************************************/
    MPI_Finalize();
}
