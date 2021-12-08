#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

// Berenice Calvario González
// Compilar: mpicc Practica2BereniceCalvario.c
// -np indica el número de procesos
// Ejecutar: mpirun -np 10 ./a.out

int main(int argc, char *argv[])
{
    int rank;
    int size;
    MPI_Status status;

    // Inicia sesión mpi.
    MPI_Init(&argc, &argv);

    // Número del proceso.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Total de procesos.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    time_t sec;
    srand(time(NULL) + rank);
    sec = rand() % 1000;

    int promedio = 0;

    if (rank == 0) // Maestro.
    {
        printf("--------Tiempo del nodo-%d-maestro %1ld--------\n", rank, sec);

        // Agregamos el tiempo del nodo maestro.
        promedio += sec;

        for (int i = 0; i < size; i++)
        {
            if (i != 0)
            {
                int received_sec;

                // Recibimos el tiempo de cada nodo esclavo en el nodo maestro.
                MPI_Recv(&received_sec, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);

                // Agregamos el tiempo de cada nodo esclavo.
                promedio += received_sec;
            }
        }

        // Sacamos el promedio de los tiempos de los nodos para obtener el nuevo tiempo "correcto".
        promedio /= size;
        printf("--Nuevo tiempo \"correcto\": %i\n", promedio);

        for (int i = 0; i < size; i++)
        {
            if (i != 0)
                // Enviamos el nuevo tiempo "correcto" a tdos los nodos esclavos.
                MPI_Send(&promedio, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Cantidad que se le tuvo que sumar al nodo maestro para tener el nuevo tiempo "correcto".
        int ajuste_realizado = promedio - sec;
        printf("-Tiempo correcto en el nodo-%d-maestro: %i, ajuste: %i\n", rank, promedio, ajuste_realizado);
    }
    else // Esclavos
    {
        printf("---Tiempo del nodo-%d-esclavo %1ld\n", rank, sec);

        // Eviamos el tiempo de cada nodo esclavo al maestro.
        MPI_Send(&sec, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        int tiempo_correcto;

        // Recibimos el nuevo tiempo "correcto" del nodos maestro.
        MPI_Recv(&tiempo_correcto, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Cantidad que se le tuvo que sumar a cada nodo esclavo para tener el nuevo tiempo "correcto".
        int ajuste_realizado = tiempo_correcto - sec;
        printf("-Tiempo correcto en el nodo-%d-esclavo: %i, ajuste sumado: %i\n", rank, tiempo_correcto, ajuste_realizado);
    }

    // Termina sesión mpi.
    MPI_Finalize();
    return 0;
}