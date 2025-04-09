Runtime Options
===============

Output file partitions (subfiles)
---------------------------------

If you're running on an IBM BG/Q supercomputer, then the number of subfiles
(partitions) chosen is based on the I/O nodes in an automatic way. Otherwise, by
default, the GenericIO library picks the number of subfiles based on a
fairly-naive hostname-based hashing scheme. This works reasonably-well on small
clusters, but not on larger systems. On a larger system, you might want to set
these environmental variables:

.. code-block:: bash

    GENERICIO_PARTITIONS_USE_NAME=0
    GENERICIO_RANK_PARTITIONS=256

Where the number of partitions (256 above) determines the number of subfiles
used. If you're using a Lustre file system, for example, an optimal number of
files is:

.. code-block::

    # of files * stripe count  ~ # OSTs

On Titan, for example, there are 1008 OSTs, and a default stripe count of 4, so
we use approximately 256 files.


All GenericIO Flags
-------------------

``GENERICIO_RANK0_CREATE_ALL``

``GENERICIO_COMPRESS``

``GENERICIO_FORCE_BLOCKS``

``GENERICIO_RETRY_COUNT``

``GENERICIO_RETRY_SLEEP``

``GENERICIO_VERBOSE``

``GENERICIO_PARTITIONS_USE_NAME``

``GENERICIO_RANK_PARTITIONS``

``GENERICIO_USE_MPIIO``

BLOSC Flags
-----------

``BLOSC_CLEVEL``

``BLOSC_SHUFFLE``

``BLOSC_TYPESIZE``

``BLOSC_COMPRESSOR``

``BLOSC_BLOCKSIZE``

``BLOSC_NTHREADS``

``BLOSC_NOLOCK``
