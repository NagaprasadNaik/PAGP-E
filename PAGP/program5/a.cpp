#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define BUFFER_SIZE 32

int main(int argc, char *argv[])
{
    int MyRank, Numprocs, Destination, tag = 0;
    int Root = 0, temp = 1;
    char Message[BUFFER_SIZE];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyRank);
    MPI_Comm_size(MPI_COMM_WORLD, &Numprocs);

    /* Rank 0 sends message to all other ranks */
    if (MyRank == 0)
    {
        system("hostname");
        strcpy(Message, "Hello India");

        for (temp = 1; temp < Numprocs; temp++)
        {
            MPI_Send(Message, BUFFER_SIZE, MPI_CHAR, temp, tag, MPI_COMM_WORLD);
        }
    }
    else
    {
        system("hostname");

        MPI_Recv(Message, BUFFER_SIZE, MPI_CHAR, Root, tag, MPI_COMM_WORLD, &status);

        printf("\n%s in process with rank %d received message from Rank %d\n",
               Message, MyRank, Root);
    }

    MPI_Finalize();
    return 0;
}
