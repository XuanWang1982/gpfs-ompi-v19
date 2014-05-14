#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {

    int rank, np;
    double read_t0, read_t1, write_t2, write_t3;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    const int filesize = 1024*1048576;  //input filesize x*MB
    const int ndoubles = filesize/(sizeof(double)*np);
    double *localdata = malloc(ndoubles*sizeof(double));

    /* create a type which describes our view of the file --
     * in particular, just our subarray of the global array
     */
    int globalsizes[1] = {filesize};
    int localsizes[1] = {ndoubles};
    int starts[1] = {ndoubles*rank};

    MPI_Datatype fileview;
    MPI_Type_create_subarray(1, globalsizes, localsizes, starts, MPI_ORDER_C, MPI_DOUBLE, &fileview);
    MPI_Type_commit(&fileview);

	MPI_Info siox_read_info, siox_write_info;
	MPI_Info_create(&siox_read_info);
	MPI_Info_create(&siox_write_info);
	char str[50];
	MPI_Info_set(siox_read_info, "useSIOXLib", "true");
/*	if (rank == 0){
		MPI_Info_set(siox_read_info, "sioxAccessRange", "0,268435455,0");
		MPI_Info_set(siox_read_info, "sioxFreeRange", "268435456,0");
	}
	if (rank == 1){
		MPI_Info_set(siox_read_info, "sioxAccessRange", "268435456,536870911,0");
		MPI_Info_set(siox_read_info, "sioxFreeRange", "536870912,0");
		MPI_Info_set(siox_read_info, "sioxFreeRange", "0,268435455");
	}
	if (rank == 2){
		MPI_Info_set(siox_read_info, "sioxAccessRange", "536870912,805306367,0");
		MPI_Info_set(siox_read_info, "sioxFreeRange", "805306368,0");
		MPI_Info_set(siox_read_info, "sioxFreeRange", "0,536870911");
	}
	if (rank == 3){
		MPI_Info_set(siox_read_info, "sioxAccessRange", "805306368,0,0");
		MPI_Info_set(siox_read_info, "sioxFreeRange", "0,805306367");
	}*/

	MPI_Info_set(siox_write_info, "useSIOXLib", "true");
/*	if (rank == 0){
		MPI_Info_set(siox_write_info, "sioxAccessRange", "0,268435455,1");
		MPI_Info_set(siox_write_info, "sioxFreeRange", "268435456,0");
	}
	if (rank == 1){
		MPI_Info_set(siox_write_info, "sioxAccessRange", "268435456,536870911,1");
		MPI_Info_set(siox_write_info, "sioxFreeRange", "536870912,0");
		MPI_Info_set(siox_write_info, "sioxFreeRange", "0,268435455");
	}
	if (rank == 2){
		MPI_Info_set(siox_write_info, "sioxAccessRange", "536870912,805306367,1");
		MPI_Info_set(siox_write_info, "sioxFreeRange", "805306368,0");
		MPI_Info_set(siox_write_info, "sioxFreeRange", "0,536870911");
	}
	if (rank == 3){
		MPI_Info_set(siox_write_info, "sioxAccessRange", "805306368,0,1");
		MPI_Info_set(siox_write_info, "sioxFreeRange", "0,805306367");
	}*/

	//siox_read_info = MPI_INFO_NULL;
	//siox_write_info = MPI_INFO_NULL;

    /* read in only our data */
    MPI_File fpa;
    MPI_Status status;
    MPI_File_open(MPI_COMM_WORLD, "/siox-gpfs/source_3", MPI_MODE_RDONLY, siox_read_info, &fpa);

    /* note could use MPI_File_seek instead of file set view */
    read_t0 = MPI_Wtime();
    MPI_File_set_view(fpa, (MPI_Offset)0, MPI_DOUBLE, fileview, "native", MPI_INFO_NULL);
    //MPI_File_read_all(fpa, localdata, ndoubles, MPI_DOUBLE, &status);
    MPI_File_read(fpa, localdata, ndoubles, MPI_DOUBLE, &status);
    read_t1 = MPI_Wtime();
    MPI_File_close(&fpa);

    /* write out data - it will have same layout, we're just writing instead of reading*/

    MPI_File_open(MPI_COMM_WORLD, "/siox-gpfs/target_3", MPI_MODE_WRONLY|MPI_MODE_CREATE, siox_write_info, &fpa);

    /* note could use MPI_File_seek instead of file set view */
    write_t2 = MPI_Wtime();
    MPI_File_set_view(fpa, (MPI_Offset)0, MPI_DOUBLE, fileview, "native", MPI_INFO_NULL);
    //MPI_File_write_all(fpa, localdata, ndoubles, MPI_DOUBLE, &status);
    MPI_File_write(fpa, localdata, ndoubles, MPI_DOUBLE, &status);
    write_t3 = MPI_Wtime();
    MPI_File_close(&fpa);

    free(localdata);
    MPI_Type_free(&fileview);
    MPI_Finalize();
	if (rank == 0){
		double time = read_t1 - read_t0;
		printf("****** Read time is: %f ******\n", time);
	}
	if (rank == 0){
		double time = write_t3 - write_t2;
		printf("****** Write time is: %f ******\n", time);
	}

    return 0;
}
