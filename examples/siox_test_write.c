#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <gpfs.h>
#include <fcntl.h>
#include <errno.h>
#include <gpfs_fcntl.h>

void main(int argc, char *argv[]) {
	int my_rank, size, len, i, j, random;
	int ndims, order, array_of_sizes[1], array_of_subsizes[1],
			array_of_starts[1];
	MPI_File fh;
	MPI_Datatype etype, filetype;
	MPI_Offset disp, filesize;
	MPI_Status status;
	double t0, t1;
	char buf;
	char file_name[30];
	char version[MPI_MAX_LIBRARY_VERSION_STRING];

	//for debugger
	/*int cont = 0;
	while (cont == 0) {
	}*/

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Get_library_version(version, &len);
	printf("Hello, world, I am %d of %d, (%s, %d)\n", my_rank, size, version,
			len);

	etype = MPI_CHAR;
	ndims = 1;
	array_of_sizes[0] = size;
	array_of_subsizes[0] = 1;
	array_of_starts[0] = my_rank;
	order = MPI_ORDER_C;

	MPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes,
			array_of_starts, order, etype, &filetype);
	MPI_Type_commit(&filetype);

	//START: setting the GPFS hints from SIOX system to MPI function
	//This part should be implemented in MPI Wrapper later

	MPI_Info siox_info;
	MPI_Info_create(&siox_info);
	char str[50];
	MPI_Info_set(siox_info, "useSIOXLib", "true");
	MPI_Info_set(siox_info, "sioxAccessRange", "0,1024,1");
	MPI_Info_set(siox_info, "sioxFreeRange", "0,1024");
	MPI_Info_set(siox_info, "sioxClearFileCache", "true");

	//END: setting the GPFS hints from SIOX system to MPI function

	for (j = 0; j < 5; j++) {
		random = rand() % 1000;
		snprintf(file_name, 30, "/siox-gpfs/siox_test_%d", j);
		printf("Creating file %s\n", file_name);
		MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDWR
				| MPI_MODE_CREATE, siox_info, &fh);
		disp = 0;
		MPI_File_set_view(fh, disp, etype, filetype, "native", MPI_INFO_NULL);

		t0 = MPI_Wtime();
		for (i = 0; i < random; i++) {
			buf = 'a' + (char) my_rank;
			MPI_File_write(fh, &buf, 1, etype, &status);
		}

		MPI_File_get_size(fh, &filesize);
		printf("File size is: %d\n", filesize);

		MPI_File_close(&fh);
		t1 = MPI_Wtime();
		if (my_rank == 0){
			double time = t1 - t0;
			printf("****** Write time is: %f ******\n", time);
		}
		printf("PE%d\n", my_rank);
	}

	MPI_Finalize();
}
