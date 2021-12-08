#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank;
    int size;
    MPI_Status status;

    MPI_Init(&argc, &argv);               // Inicia sesión mpi
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Número del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Total de procesos de comunicador

    time_t sec;
    srand(time(NULL) + rank);
    sec = rand() % 1000;

    if (rank == 0) // Maestro
    {
        printf("--------Tiempo del nodo-%d-maestro %1ld--------\n", rank, sec);

        for (int i = 0; i < size; i++)
        {
            if (i != 0)
            {
                int received_sec;
                MPI_Recv(&received_sec, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                printf("Tiempo recibido en el nodo-maestro del nodo-%d-esclavo %1d\n", i, received_sec);
                int dif = sec - received_sec;
                printf("El nodo-%d-esclavo tiene diferencia de %d\n", i, dif);
                MPI_Send(&dif, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
    }
    else // Esclavos
    {
        printf("Tiempo del nodo-%d-esclavo %1ld\n", rank, sec);

        MPI_Send(&sec, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        int dif;
        MPI_Recv(&dif, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Tiempo de ajuste recibido del nodo-maestro en el nodo-%d-esclavo %1d\n", rank, dif);

        int tiempo_nuevo = sec + dif;
        printf("Tiempo ajustado del nodo-%d-esclavo %d\n", rank, tiempo_nuevo);
    }

    MPI_Finalize(); // Termina sesión mpi
    return 0;
}