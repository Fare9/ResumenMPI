# ResumenMPI

Hardware:

    -maquinas = computadores completos(proc + mem), E/S.

    -Interconexión por redes.

    -Comunicación por operaciones E/S.

Punto de vista programación:

    -Acceso direcciones privadas(mem. local)

    -Comunicación intercambio mensajes.

Intervención de SSOO, uso de librerías.

MPI ---> "Message Passing Interface" (interfaz paso de mensajes)
Funciones en C (subrutinas FORTRAN), programa con paso de mensajes. Ejecución coordinada programa en múltiples procesadores, memoria distribuida.
MPI está normalizado para máquinas con MPI instalado.

Cabecera necesaria: "mpi.h" .Con ello tendremos funciones y constantes definidas.

Funciones y constantes empiezan por MPI_ (más el nombre).

    -Funciones: MPI_ + nombre que empieza por letra mayúscula.

    -Constantes: MPI_ + nombre todo en mayusculas.

Funciones devuelven entero.

Antes de comenzar programa se debe hacer llamada a MPI_Init y antes de acabar se llama a MPI_Finalize:



int main(int argc, char *argv[]){

    MPI_Init(&argc,&argv);
    /*****************************/

    //a partir de aqui se pueden llamar funciones MPI


    //a partir de aqui ya no se pueden llamar funciones MPI
    /*****************************/
    MPI_Finalize()
    
}

-Grupo de comunicación (IMPORTANTE): familia de procesos, con permiso para intercambio mensajes. Existe uno por defecto MPI_COMM_WORLD, con todos los procesos en momento de arranque.
En cada grupo cada proceso tiene id que se obtiene con MPI_Comm_rank. Número total procesos en grupo se consigue con MPI_Comm_size.

Para envío de mensajes:
    MPI_Send
    MPI_Recv

Mensajes solo comunican dentro de mismo grupo. Cada mensaje lleva un tag identificado único. 

# Ejemplo MPI
Antes de ejecutar, cada máquina debe tener acceso a copia o tener disco compartido. A la hora de ejecutar cada máquina comienza ejecución independiente.

(Ejemplo en MPI1.c)


#Funciones MPI:


    int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);

datatype puede ser:


    -MPI_CHAR        signed char

    -MPI_SHORT       signed short int

    -MPI_FLOAT       float

    -MPI_INT         signed int

    -MPI_DOUBLE      double
    


    int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)

El parametro count, especifica número máximo elementos de datatype que caben en buffer buf. Si se envía mensaje más largo hay buffer overflow.
MPI_Status tiene siguientes campos:
    MPI_SOURCE
    MPI_TAG
    MPI_ERROR

Contiene además información sobre tamaño de mensaje recibido, se obtiene mediante:

    MPI_Get_count (MPI_Status *status,MPI_Datatype datatype,int *cuenta);

Cuenta obtendrá el número de elementos de tipo datatype (no número de bytes, eso podría obtenerse como cuenta*sizeof(datatype) ).

    MPI_Comm_rank(MPI_Comm comunicador,int *rango);

Devuelve en rango el id del comunicador, comunicador es estructura agrupa procesos que pueden intercambiar mensajes.

    MPI_Comm_size(MPI_Comm comunicador,int *tamanyo);

Devuelve en tamanyo el número procesos en comunicador.


#Comunicación colectiva

int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )

Puede resultar en:
    -Recibir datos, si rango proceso es distinto de root.
    -Envío datos, si rango proceso es igual a raiz.

Valor datatype y count deben ser iguales en todos los procesos.
Procesos afectados serán aquellos que estén en comm.
MPI garantiza que si se realizan varias llamadas a MPI_Bcast serán recibidas en orden que fueron emitidos.

(Ejemplo integración con trapecios en trap.c)
Programa tiene dos fases:
    -Calculo de integral
    -Suma de resultados

Primera fase está distribuida entre procesos, la suma solo la realiza el proceso 0, podríamos distribuir la suma entre los procesos.

    int MPI_Reduce(const void *sendbuf, void *recvbuf, int count,MPI_Datatype datatype,MPI_Op op, int root, MPI_Comm comm)

Combina operandos almacenados en sendbuf, usando la operación op, y almacena resultado en recvbuf en el proceso root.Tanto sendbuf como recvbuf se refieren a count elementos de tipo datatype. Esta función debe ser llamada en todos los procesos del grupo comm.

op puede ser:
    MPI_SUM
    MPI_PROD
    MPI_MAX
    MPI_MIN
    ...

resultado solo tiene sentido en raiz, pero los demás también han de especificarla.

(Ejemplo de uso en trap2.c)

Hay casos que queremos que todos los procesos obtengan el resultado:
    int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count,MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
Al guardarse el resultado en todos, no hace falta el parámetro raíz.

Las barreras están ya en MPI:
    MPI_Barrier(MPI_Comm grupo_com);

Procesos al llegar esperan hasta que todos los procesos de grupo_com llegan. Al llegar todos se desbloquean simultaneamente.

    MPI_Gather(void *buffer_envio,int cuenta_envio,MPI_Datatype tipo_envio, void *buffer recepcion,int cuenta_recepcion,MPI_Datatype tipo_recepcion,int raiz,MPI_Comm grupo_com);

Cada proceso del grupo envía contenidos buffer_envio a raiz. Raiz concatena datos en buffer_recepcion.(Los de 0...Los de 1...). Cuenta_recepcion indica numero items recibidos de cada proceso(no el total)

    MPI_Scatter(void *buffer_envio,int cuenta_envio,MPI_Datatype tipo_envio, void *buffer_recepcion, int cuenta_recepcion, MPI_Datatype tipo_recepcion, int raiz, MPI_Comm grupo_com);

Proceso con rango raiz distribuye buffer_envio en segmentos como procesos haya, cada uno tamaño = cuenta_envio. 


#Agrupación de datos

Enviar mensajes costoso, intentar enviar mínimo posible. Solución agrupar mensajes. Tres formas:
    -Parametro cuenta.
    -Tipos de datos derivados.
    -Rutinas MPI_Pack/MPI_Unpack.

Las funciones anteriores usaban todas cuenta y tipodato. Usuario se limita a mismo tipo básico. Los datos deben estar en memoria contigua(arrays estáticos) Ejemplo enviando segunda mitad de un array.

float vector[100];

if (mi_rango == 0){
    MPI_Send(vector+50,50,MPI_FLOAT,1,0,MPI_COMM_WORLD);
}else if(mi_rango == 1){
    MPI_Recv(vector+50,50,MPI_FLOAT,0,0,MPI_COMM_WORLD,&status);
}

Si datos no son mismo tipo, se construyen tipos derivados.
Tipo MPI derivado = sucesión de pares.
    {(t0,d0),(t1,d1),...,(tn-1,dn-1)}
    cada ti es tipo basico y cada di es un desplazamiento en bytes.
Ejemplo:
    {(MPI_FLOAT,0),(MPI_FLOAT,16),(MPI_INT,24)}

Función para construir tipo:
    
    MPI_Type_struct(int cuenta,int long_bloque[],MPI_Aint desplazamiento[],MPI_Datatype lista_de_tipos[],MPI_Datatype *nuevo_tipo);

Cuenta nº elementos tipo derivado y tamaño de vectos long_bloque,desplazamiento y lista_de_tipos.
Lista_de_tipos: contiene tipo dato MPI para cada entrada
Desplazamiento: desplazamiento respecto comienzo mensaje de cada entrada.
Long_bloque: cuántos elementos de cada tipo hay en cada entrada.

Ejemplo:
    float a;
    float b;
    int n;

    int long_bloque[3];
    MPI_Aint desplazamientos[3];
    MPI_Datatype lista_de_tipos[3];

    //para cálculos de direcciones
    MPI_Aint start_address;
    MPI_Aint address;

    //Nuevo tipo
    MPI_Datatype nuevo_tipo;

    //nuestros datos son de un solo elemento cada uno
    long_bloque[0] = long_bloque[1] = long_bloque[2] = 1;

    //variables son dos flotantes y un entero
    typelist[0] = typelist[1] = MPI_FLOAT;
    typelist[2] = MPI_INT;

    //primer elemento lo ponemos a desplazamiento 0
    desplazamientos[0] = 0;

    //calculamos los otros
    MPI_Address(&a,&start_address);
    MPI_Address(&b,&address);
    desplazamientos[1] = address - start_address;
    MPI_Address(&n,&address);
    desplazamientos[2] = address - start_address;

    //construimos tipo derivado
    MPI_Type_struct(3,long_bloque,desplazamientos,lista_de_tipos,&nuevo_tipo);

    //informamos de nuevo tipo
    MPI_Type_commit(&nuevo_tipo)

Dos funciones más:
    
Igual a direccion = &variable,asegura portabilidad:
    
    MPI_Address(void *variable,MPI_Aint *direccion);

Para que SSOO realice cambios en el tipo nuevo:
    
    MPI_Type_commit(MPI_Datatype *nuevo_tipo);

---------------------------------------------------------
Constructor tipos más sencillos:
    
    MPI_Type_contiguous(int cuenta,MPI_Datatype tipo_viejo,MPI_Datatype *tipo_nuevo);

Cuenta elementos contiguos de vectos de elementos de tipo = tipo_viejo.

    float A[10][10]
    MPI_Datatype tipo_fila_10;

    MPI_Type_contiguous(10,MPI_FLOAT, &tipo_fila_10);
    MPI_Type_commit(&tipo_fila_10);
    if (mi_rango == 0){
        //envio toda la fila 2
        MPI_Send(&(A[2][0]), 1, tipo_fila_10, 1, 0, MPI_COMM_WORLD);
    } else{
        MPI_Recv(&(A[2][0]),1,tipo_fila_10,0,0,MPI_COMM_WORLD,&status);
    }

El anterior era para memoria contigua, pero podemos tener un tipo de dato que coja tipos de datos espaciados.

MPI_Type_vector(int cuenta,int long_bloque,int espaciado,MPI_Datatype tipo_elem, MPI_Datatype *tipo_nuevo);


Por último podemos empaquetar datos y desempaquetarlos al recibir:

    MPI_Pack(void *paquete,int cuenta, MPI_Datatype tipo_dato,void *buffer,int tamanyo_buffer,int *posicion,MPI_Comm grupo_com);

    MPI_Unpack(void *buffer,int tamanyo_buffer,int *posicion,void *desempaquetado,int cuenta, MPI_Datatype tipo_dato,MPI_Comm grupo_com);

Ejemplo de uso:

    float a;
    float b;
    int n;
    char buffer[100];
    int posicion = 0;

    if (mi_rango == 0){

        MPI_Pack(&a,1,MPI_FLOAT,buffer,100,&posicion,MPI_COMM_WORLD);

        MPI_Pack(&b,1,MPI_FLOAT,buffer,100,&posicion,MPI_COMM_WORLD);

        MPI_Pack(n,1,MPI_INT,buffer,100,&posicion,MPI_COMM_WORLD);

        //difundo contenido buffer
        MPI_Bcast(buffer,100,MPI_PACKED,0,MPI_COMM_WORLD);
    } else {
        //recibo buffer
        MPI_Bcast(buffer,100,MPI_PACKED,0,MPI_COMM_WORLD);

        //desempaquetamos
        MPI_Unpack(buffer,100,&posicion,&a,1,MPI_FLOAT,MPI_COMM_WORLD);
        MPI_Unpack(buffer,100,&posicion,&b,1,MPI_FLOAT,MPI_COMM_WORLD);
        MPI_Unpack(buffer,100,&posicion,&n,1,MPI_INT,MPI_COMM_WORLD);
    }

