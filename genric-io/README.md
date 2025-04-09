[![Latest Release](https://git.cels.anl.gov/hacc/genericio/-/badges/release.svg)](https://git.cels.anl.gov/hacc/genericio/-/releases)
[![pipeline status](https://git.cels.anl.gov/hacc/genericio/badges/master/pipeline.svg)](https://git.cels.anl.gov/hacc/genericio/-/commits/master)

# GenericIO

GenericIO is a write-optimized library for writing self-describing scientific
data files on large-scale parallel file systems.

* Repository: [git.cels.anl.gov](https://git.cels.anl.gov/hacc/genericio)
* Documentation: [CPACdocs](https://www.hep.anl.gov/CPACdocs/genericio/)

## Reference

Habib, et al., HACC: Simulating Future Sky Surveys on State-of-the-Art
Supercomputing Architectures, New Astronomy, 2015
(http://arxiv.org/abs/1410.2805).

## Obtaining the Source Code

The most recent version of source is available by cloning this repo:
```bash
git clone https://git.cels.anl.gov/hacc/genericio.git
```

There is also a history of code
[releases](https://xgitlab.cels.anl.gov/hacc/genericio/-/releases):
[2019-04-17](https://xgitlab.cels.anl.gov/hacc/genericio/-/releases/20190417) /
[2017-09-25](https://xgitlab.cels.anl.gov/hacc/genericio/-/releases/20170925) /
[2016-08-29](https://xgitlab.cels.anl.gov/hacc/genericio/-/releases/20160829) /
[2016-04-12](https://xgitlab.cels.anl.gov/hacc/genericio/-/releases/20160412) /
[2015-06-08](https://xgitlab.cels.anl.gov/hacc/genericio/-/releases/20150608) /

-----

## Building Executables / C++Library

The executables and  ``libgenericio`` can be built either with
[CMake](https://cmake.org/) (minimum version 3.10) or with
[GNUMake](https://www.gnu.org/software/make/). The following executables will
be built:

- ``frontend/GenericIOPrint`` print data to stdout (non-MPI version)
- ``frontend/GenericIOVerify`` verify and try reading data (non-MPI version)
- ``mpi/GenericIOBenchmarkRead`` reading benchmark, works on data written with ``GenericIOBenchmarkWrite``
- ``mpi/GenericIOBenchmarkWrite`` writing benchmark
- ``mpi/GenericIOPrint`` print data to stdout
- ``mpi/genericIORewrite`` rewrite data with a different number of ranks
- ``mpi/genericIOVerify`` verify and try reading data

**Using CMake**

Note that the executables / libraries will be located in
``build/<frontend/mpi>``. CMake will use the compiler pointed to in the ``CC``
and ``CXX`` environmental variables.

```bash
mkdir build && cd build
cmake ..
make -j4
```

**Using Make**

Make will create the executables / libraries under the main directory. Edit the
``CC``, ``CXX``, ``MPICC``, and ``MPICXX`` variables in the GNUmakefile to
change the compiler.

```bash
make
```

## Installing the Python Library

The `pygio` library is pip-installable and works with `mpi4py`.

**Requirements**

- Currently, a **CMake version >= 3.11.0** is required to fetch dependencies
during configuration. If the system does not provide a suitable `cmake` version,
`pip` should (theoretically) download `cmake` from the PyPI repository.
- The ``pygio`` module also requires MPI libraries to be detectable by CMake's
FindMPI. The compiler needs to support **C++17** (make sure that ``CC`` and
``CXX`` point to the correct compiler).

**Install**

The python library can be installed by running pip in the **main folder**:
```bash
python -m pip install .
```
Alternatively, the library can also directly be installed from the git URL
without having to clone the repository first:
```bash
python -m pip install git+https://git.cels.anl.gov/hacc/genericio.git
```

It will use the compiler referred by the ``CC`` and ``CXX`` environment
variable. In case the automatically detected compiler is incorrect, specify the
compiler path as

```bash
CC=/path/to/gcc CXX=/path/to/g++ python -m pip install .
```

If the compiler supports OpenMP, the library will be threaded. Make
sure to set ``OMP_NUM_THREADS`` to an appropriate variable, in particluar when
using multiple MPI ranks per node.

## Installing and running with VELOC support

**Requirements**

This mode requires a working VELOC installation.
Instructions can be found here: [https://veloc.readthedocs.io](https://veloc.readthedocs.io)

**Install**

Set the VELOC_INSTALL_DIR variable in GNUMakefile to the root of the VELOC installation directory.
Then proceed to compile and link GIO as usual.

**Run**

Define the GENERICIO_USE_VELOC environment variable as the path to the scratch directory.
The scratch directory will be used as a local cache and needs to be a NVMe mount point on the compute node.
Define the VELOC_MAX_CACHE_SIZE environment variable as the maximum size (in bytes) of unflushed data allowed in the scratch folder.
