/*
    Programa que usa MPI para mostrar ejemplo de uso, todos los programas
    que no tienen ID = 0 enviarán un saludo al proceso con ID = 0.
*/

#include <stdio.h>
#include <string.h>
#include "mpi.h"


int main(int argc, char *argv[]){
    int mi_rango;   //id del proceso
    int p;  //numero de procesos
    int fuente; //id del remitente
    int dest;   //id destinatario
    int tag = 0;    //etiqueta del mensaje
    char mensaje[256];  //buffer de mensajes
    MPI_Status  status; //status de return


    MPI_Init(&argc,&argv); //arranco MPI
    /*********************************/
    //a partir de aqui puedo usar funciones MPI

        MPI_Comm_rank(MPI_COMM_WORLD,&mi_rango); //saco mi ID

        MPI_Comm_size(MPI_COMM_WORLD,&p); //saco el numero de procesos

        if (mi_rango != 0){
            /*
                Si no soy el proceso con id = 0
                envío mensaje.
            */

            sprintf(mensaje,"Saludos desde el proceso: %d",mi_rango);
            dest = 0;
            //ahora envio el mensaje, con strlen+1 para incluir '\0'
            MPI_Send(mensaje,strlen(mensaje)+1,MPI_CHAR,dest,tag,MPI_COMM_WORLD)

        } else {
            /*
                Si el id del proceso id = 0, recibo todos los mensajes
                y los muestro
            */
            for (fuente = 1; fuente < p; fuente++){
                MPI_Recv(mensaje,sizeof(mensaje),MPI_CHAR,fuente,tag,MPI_COMM_WORLD,&status);
                printf("%s\n",mensaje);
            }
        }

    /*************************************/
    MPI_Finalize();
    
}
