Reading and Writing Data
========================

.. currentmodule:: pygio

The ``pygio`` module contains a high-level functional interface for inspecting,
reading, and writing particle and halo data. In addition, there is a class-based
interface which mimics the C++ ``GenericIO`` class. More information about
:class:`PyGenericIO` can be found :doc:`here <./class_interface>`.

.. note::

   The GenericIO python library supports **MPI** with
   `mpi4py <https://mpi4py.readthedocs.io/en/stable/>`_. Under MPI, each rank
   will read a separate "chunk" of the file (if the file was written with
   multiple ranks). The function :func:`read_num_elems` will return the number
   of particles / halos that the local rank will read and can be different on
   each rank. More information on how to use this library with MPI can be found
   :doc:`here <./mpi>`.

To get an overview of the data contained in a GenericIO file, use the
:func:`inspect_genericio` function, which lists all variables, data types and
number of elements (particles or halos) contained in the file.

.. code-block:: python

   import pygio

   # inspect file
   pygio.inspect_genericio("generic_io_file")

Some additional inspection functions are:

- :func:`read_num_elems` returns the number of elements in the file
- :func:`read_total_num_elems` returns the total/global number of elements in the file
- :func:`read_variable_names` returns a list of variable names defined in the file
- :func:`read_variable_dtypes` returns a dictionary with ``variable_name -> numpy.dtype``
- :func:`read_phys_scale` returns a list of 3 floats describing the box size
- :func:`read_phys_origin` returns a list of 3 floats describing the box origin


Reading GenericIO files
-----------------------

Data contained in GenericIO files can be loaded into numpy arrays using the
:func:`read_genericio` function. By default, all variables stored in the
file will be loaded. If not all variables are needed, the memory footprint and
read speed can be improved by passing a list of variable names to to the
function.

.. code-block:: python

   # read all variables
   data = pygio.read_genericio("generic_io_file")

.. code-block:: python

   # read only a subset of variables
   data_partial = pygio.read_genericio("generic_io_file", ["x", "y", "z"])
   data_x = data_partial["x"]


Writing GenericIO files
-----------------------

Data arrays can be stored into GenericIO files using the :func:`write_genericio`
function. The data has to be a dictionary, with the variable name as key and the
corresponding one dimensional numpy array as value. The numpy arrays have to
meet the following criteria:

- 1 dimensional
- same length
- datatype has to be ``np.float32``, ``np.float64``, ``np.int64`` or
  ``np.uint16``

In addition, one needs to specify the physical origin (default [0, 0, 0]) and
the physical box size as a list of 3 floats.

.. code-block:: python

    data = {
        "x": np.random.uniform(0, 1, 100, dtype=np.float32),
        "y": np.random.uniform(0, 1, 100, dtype=np.float32),
        "z": np.random.uniform(0, 1, 100, dtype=np.float32)
    }

   # write data to file
   pygio.write_genericio("new_file.gio",
        data,
        phys_scale = [1, 1, 1],
        phys_origin = [0, 0, 0]
    )


References
----------

.. autofunction:: inspect_genericio

.. autofunction:: read_genericio

.. autofunction:: read_num_elems

.. autofunction:: read_total_num_elems

.. autofunction:: read_variable_names

.. autofunction:: read_variable_dtypes

.. autofunction:: read_phys_scale

.. autofunction:: read_phys_origin

.. autofunction:: write_genericio

.. autoclass:: FileIO
   :noindex:

.. autoclass:: MismatchBehavior
   :noindex:

.. autofunction:: setDefaultShouldCompress

.. autofunction:: setNaturalDefaultPartition

.. autofunction:: setCollectiveMPIIOThreshold
