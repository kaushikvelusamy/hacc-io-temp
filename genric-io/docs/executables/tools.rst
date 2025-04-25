Executables
===========

- ``frontend/GenericIOPrint`` print data to stdout (non-MPI version)
- ``frontend/GenericIOVerify`` verify and try reading data (non-MPI version)
- ``mpi/GenericIOPrint`` print data to stdout
- ``mpi/GenericIORewrite`` rewrite data with a different number of ranks
- ``mpi/GenericIOVerify`` verify and try reading data
- ``mpi/GenericIOBenchmarkRead`` reading benchmark, works on data written
  with ``GenericIOBenchmarkWrite``
- ``mpi/GenericIOBenchmarkWrite`` writing benchmark

--------------------------------------------------------------------------------

.. _doc-GenericIOPrint:

GenericIOPrint
--------------

inspect GenericIO file and print data to stdout

.. code-block:: none

    Usage: GenericIOPrint [--no-rank-info|--no-data|--show-map] <filename>

    Options:
        --no-rank-info    don't print source rank information (coords and #rows)
        --no-data         don't print data
        --show-map        ???

--------------------------------------------------------------------------------

.. _doc-GenericIOVerify:

GenericIOVerify
---------------

verify and try reading data

.. code-block:: none

    Usage: GenericIOVerify [-v] <filename1> [<filename2> ...]

    Options:
        -v          verbose

--------------------------------------------------------------------------------

.. _doc-GenericIORewrite:

GenericIORewrite
----------------

rewrite data with a different number of ranks

.. code-block:: none

    Usage: GenericIORewrite <filenameOld> <filenameNew>

--------------------------------------------------------------------------------

.. _doc-GenericIOBenchmarkRead:

GenericIOBenchmarkRead
----------------------

reading benchmark, works on data written with ``GenericIOBenchmarkWrite``

.. code-block:: none

    Usage: GenericIOBenchmarkRead [-a] <filename>

    Options:
        -a          UseAOS (Array-Of-Structures for pos and vel)

--------------------------------------------------------------------------------

.. _doc-GenericIOBenchmarkWrite:

GenericIOBenchmarkWrite
-----------------------

writing benchmark

.. code-block:: none

    Usage: GenericIOBenchmarkWrite [-a] [-c] [-l] <filename> <NP> <seed>

    Options:
        -a          UseAOS (Array-Of-Structures for pos and vel)
        -c          compress data
        -l          UseLC (Lossy Compression)

