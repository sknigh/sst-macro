/**
Copyright 2009-2018 National Technology and Engineering Solutions of Sandia, 
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S.  Government 
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly 
owned subsidiary of Honeywell International, Inc., for the U.S. Department of 
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2018, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of Sandia Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact sst-macro-help@sandia.gov
*/

#include <sstmac/replacements/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpitest.h"

namespace redscatinter {

int redscatinter( int argc, char **argv )
{
    int      err = 0;
    int      *recvcounts;
    int      size, rsize, rank, i;
    int      recvcount, /** Each process receives this much data */
             sendcount, /** Each process contributes this much data */
	     basecount; /** Unit of elements - basecount *rsize is recvcount, 
			   etc. */
    int      isLeftGroup;
    long long *sendbuf, *recvbuf;
    long long sumval;
    MPI_Comm comm;


    MTest_Init( &argc, &argv );
    comm = MPI_COMM_WORLD;

    basecount = 1024;

    while (MTestGetIntercomm( &comm, &isLeftGroup, 2 )) {
	if (comm == MPI_COMM_NULL) continue;

	MPI_Comm_remote_size( comm, &rsize );
	MPI_Comm_size( comm, &size );
	MPI_Comm_rank( comm, &rank );

	if (0) {
	    printf( "[%d] %s (%d,%d) remote %d\n", rank, 
		    isLeftGroup ? "L" : "R", 
		    rank, size, rsize );
	}

	recvcount = basecount * rsize;
	sendcount = basecount * rsize * size;

	recvcounts = (int *)malloc( size * sizeof(int) );
	if (!recvcounts) {
	    fprintf( stderr, "Could not allocate %d int for recvcounts\n", 
		     size );
	    MPI_Abort( MPI_COMM_WORLD, 1 );
	}
	for (i=0; i<size; i++) 
	    recvcounts[i] = recvcount;
	
	sendbuf = (long long *) malloc( sendcount * sizeof(long long) );
	if (!sendbuf) {
	    fprintf( stderr, "Could not allocate %d ints for sendbuf\n", 
		     sendcount );
	    MPI_Abort( MPI_COMM_WORLD, 1 );
	}

	for (i=0; i<sendcount; i++) {
	    sendbuf[i] = (long long)(rank*sendcount + i);
	}
	recvbuf = (long long *)malloc( recvcount * sizeof(long long) );
	if (!recvbuf) {
	    fprintf( stderr, "Could not allocate %d ints for recvbuf\n", 
		     recvcount );
	    MPI_Abort( MPI_COMM_WORLD, 1 );
	}
	for (i=0; i<recvcount; i++) {
	    recvbuf[i] = (long long)(-i);
	}
	
	MPI_Reduce_scatter( sendbuf, recvbuf, recvcounts, MPI_LONG_LONG, MPI_SUM,
			    comm );

	/** Check received data */
	for (i=0; i<recvcount; i++) {
	    sumval = (long long)(sendcount) * (long long)((rsize * (rsize-1))/2) +
		(long long)(i + rank * rsize * basecount) * (long long)rsize;
	    if (recvbuf[i] != sumval) {
		err++;
		if (err < 4) {
		    fprintf( stdout, "Did not get expected value for reduce scatter\n" );
		    fprintf( stdout, "[%d] %s recvbuf[%d] = %lld, expected %lld\n",
			     rank, 
			     isLeftGroup ? "L" : "R", 
			     i, recvbuf[i], sumval );
		}
	    }
	}
	
	free(sendbuf);
	free(recvbuf);
	free(recvcounts);

	MTestFreeComm( &comm );
    }

    MTest_Finalize( err );

    MPI_Finalize( );

    return 0;
}

}