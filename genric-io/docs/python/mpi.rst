Using GenericIO with MPI
========================

The `pygio` library is working with mpi4py. Here is an example:

.. code-block:: python

    from mpi4py import MPI
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    ranks = comm.Get_size()

    import numpy as np
    import pygio

    # read locally
    data = pygio.read_genericio("generic_io_file")
    # get local number of elements from the first element in dictionary
    num_elems = len(next(iter(data.values())))
    # reduce total number of elements
    num_elems_total = comm.allreduce(num_elems)
    if rank == 0:
        print(f"Reading file with {ranks} ranks")
        print(f"Total number of particles: {num_elems_total}")
        print("The data contains the following variables:")
        for k, d in data.items():
            print(f"\t{k:5s}, dtype={d.dtype}")

    for i in range(ranks):
        if i == rank:
            print(f"rank {rank} read {num_elems} elements")
        comm.Barrier()


It can be executed with ``mpirun`` or ``mpiexec``:

.. code-block:: bash

    mpirun -n 8 python testpygio_mpi.py

Here is an output for 1 and 8 ranks:

.. code-block:: none

    # 1 Rank
    Reading file with 1 ranks
    Total number of particles: 562500
    The data contains the following variables:
            id   , dtype=int64
            vx   , dtype=float32
            vy   , dtype=float32
            vz   , dtype=float32
            x    , dtype=float32
            y    , dtype=float32
            z    , dtype=float32
    rank 0 read 562500 elements

    # 8 Ranks
    Reading file with 8 ranks
    Total number of particles: 562500
    The data contains the following variables:
            id   , dtype=int64
            vx   , dtype=float32
            vy   , dtype=float32
            vz   , dtype=float32
            x    , dtype=float32
            y    , dtype=float32
            z    , dtype=float32
    rank 0 read 70000 elements
    rank 1 read 70000 elements
    rank 2 read 70000 elements
    rank 3 read 70000 elements
    rank 4 read 70625 elements
    rank 5 read 70625 elements
    rank 6 read 70625 elements
    rank 7 read 70625 elements


Importing the non-MPI version
-----------------------------

Some compute facilities prevent the loading of MPI libraries on login nodes. In order
to still be able to use ``pygio`` on the login nodes, the non-MPI library
can be loaded by setting the `GENERICIO_NO_MPI` environment variable before
importing `pygio`:

.. code-block:: python

   import os
   os.environ['GENERICIO_NO_MPI'] = 'True'
   import pygio

A warning will be printed that the writing capabilities of genericio are not
available in this mode.