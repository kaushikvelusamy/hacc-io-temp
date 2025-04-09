Benchmarks
==========

Once you build the library and associated programs (using make), you can run, for example:

.. code-block::

    $ mpirun -np 8 ./mpi/GenericIOBenchmarkWrite /tmp/out.gio 123456 2
    Wrote 9 variables to /tmp/out (4691036 bytes) in 0.2361s: 18.9484 MB/s

.. code-block::

    $ mpirun -np 8 ./mpi/GenericIOBenchmarkRead /tmp/out.gio
    Read 9 variables from /tmp/out (4688028 bytes) in 0.223067s: 20.0426 MB/s [excluding header read]

The read benchmark always reads all of the input data. The output benchmark takes two numerical parameters, one if the
number of data rows to write, and the second is a random seed (which slightly perturbs the per-rank output sizes, but
not by much). Each row is 36 bytes for these benchmarks.

The write benchmark can be passed the -c parameter to enable output compression. Both benchmarks take an optional -a
parameter to request that homogeneous aggregates (i.e. "float4") be used instead of using separate arrays for each
position/velocity component.

