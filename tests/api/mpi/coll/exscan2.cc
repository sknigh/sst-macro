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

namespace exscan2{
/**
static char MTEST_Descrip[] = "Test MPI_Exscan (simple test)";
*/

int exscan2( int argc, char *argv[] )
{
    int errs = 0;
    int rank, size;
    int sendbuf[1], recvbuf[1];
    MPI_Comm      comm;

    MTest_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;
    MPI_Comm_rank( comm, &rank );
    MPI_Comm_size( comm, &size );
    
    sendbuf[0] = rank;
    recvbuf[0] = -2;
	    
    MPI_Exscan( sendbuf, recvbuf, 1, MPI_INT, MPI_SUM, comm );

    /** Check the results.  rank 0 has no data.  Input is
       0  1  2  3  4  5  6  7  8 ...
       Output is
       -  0  1  3  6 10 15 21 28 36
       (scan, not counting the contribution from the calling process)
    */
    if (rank > 0) {
	int result = (((rank) * (rank-1))/2);
	/** printf( "%d: %d\n", rank, result ); */
	if (recvbuf[0] != result) {
	    errs++;
	    fprintf( stderr, "Error in recvbuf = %d on %d, expected %d\n",
			 recvbuf[0], rank, result );
	}
    }
    else if (recvbuf[0] != -2) {
	errs++;
	fprintf( stderr, "Error in recvbuf on zero, is %d\n", recvbuf[0] );
    }

    MTest_Finalize( errs );
    MPI_Finalize();
    return 0;
}

}