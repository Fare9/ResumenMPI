/*
    Integración mediante método por trapecios.
    Algoritmo:

    1.  Cada proceso se asigna propio intervalo integración.
    2.  Cada proceso integra f(x) en propio intervalo con método de trapecios
    3a. Cada proceso con rango != 0 envia resultado a proceso 0
    3b. Proceso 0 suma resultados de cálculos realizados por los demás e imprime resultado
*/

#include <stdio.h>
#include "mpi.h"


/*prototipo de función que integra */

float Trap(float local_a, float local_b, int local_n, float h);


int main(int argc, char *argv[]){
    int mi_rango;   //id proceso
    int p;          //num procesos total
    float a = 0.0;  //extremo izquierda
    float b = 1.0;  //extremo derecho
    int n = 1024;   //numero trapecios
    float h;        //base de cada trapecio
    float local_a;  //extremo izquiero de proceso
    float local_b;  //extremo derecho de proceso
    int local_n;    //numero trapecios para mi calculo
    float integral; //resultado integral de mi intervalo
    float total;    //resultado integral total
    int fuente;     //proceso que remite el resultado
    int dest = 0;   //Todos los resultados van al proceso 0
    MPI_Status status;

    MPI_Init(&argc,&argv);  //arranco MPI
    /*******************************************/
    //a partir de aqui puedo usar funciones de MPI

        MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango); //obtengo mi id
        MPI_Comm_size(MPI_COMM_WORLD, &p); //obtengo nº total procesos

        h = (b-a)/n;    //base de trapecios igual para todos los procesos
        local_n = n/p;  //mi numero de trapecios es el numero de trapecios entre numero de procesos

        /*
            La longitud del intervalo es igual a local_n*h.
        */
        local_a =   a + mi_rango*local_n*h;
        local_b =   local_a + local_n*h;

        //se calcula la integral con los valores 
        integral = Trap(local_a,local_b,local_n,h);

        MPI_Reduce(&integral,&total,1,MPI_FLOAT,dest,tag,MPI_COMM_WORLD)

        if (mi_rango == 0){
            //si soy el proceso con id = 0 imprimo el resultado
            printf("Con n = %d trapecios, la integral de f(x) ",n);
            printf("desde %f a %f = %f\n",a,b,total);
        }

    /*******************************************/
    MPI_Finalize();

}


/* funcion que realiza el calculo */

float Trap(float local_a,float local_b, int local_n, float h){

    float integral; //para almacenar resultado
    float x;
    int i;
    float f(float x); //funcion a integrar

    integral = (f(local_a) + f(local_b)) / 2.0;
    x = local_a;
    for (i = 1; i <= local_n-1; i++){
        x = x+h;
        integral = integral + f(x);
    }
    integral = integral * h;
    return integral;
}

/*
    Funcion que vamos a integrar. Metemos el codigo que corresponda a la función
    que deseamos integrar
*/

float f(float x) {
    float return_val;
    /* calculamos f(x) y lo almacenamos en return_val */

    return return_val;
}