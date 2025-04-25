from __future__ import print_function
import os
from ._version import __version__

_GENERICIO_NO_MPI = False
if "GENERICIO_NO_MPI" in os.environ:
    _GENERICIO_NO_MPI = os.environ["GENERICIO_NO_MPI"].lower() in ["true", "yes", "y"]

if _GENERICIO_NO_MPI:
    print(
        "WARNING: the pygio module without MPI support has been loaded (due to the "
        "GENERICIO_NO_MPI env variable). Writing GenericIO files not supported."
    )
    from . import pygio_nompi_impl as _pygio_impl

else:
    # try to load the MPI library
    try:
        from . import pygio_impl as _pygio_impl
    except ImportError:
        print(
            "WARNING: The pygio module has been compiled without MPI support. Writing "
            "GenericIO files not supported."
        )
        _GENERICIO_NO_MPI = True
        from . import pygio_nompi_impl as _pygio_impl
    else:
        # MPI version successfully loaded -> init MPI
        _pygio_impl._init_mpi()

PyGenericIO = _pygio_impl.PyGenericIO
read_genericio = _pygio_impl.read_genericio
inspect_genericio = _pygio_impl.inspect_genericio
read_num_elems = _pygio_impl.read_num_elems
read_total_num_elems = _pygio_impl.read_total_num_elems
read_variable_names = _pygio_impl.read_variable_names
read_variable_dtypes = _pygio_impl.read_variable_dtypes
read_phys_scale = _pygio_impl.read_phys_scale
read_phys_origin = _pygio_impl.read_phys_origin

if not _GENERICIO_NO_MPI:
    write_genericio = _pygio_impl.write_genericio


# move some ENUMs and static functions up to the module namespace
FileIO = PyGenericIO.FileIO
MismatchBehavior = PyGenericIO.MismatchBehavior
setDefaultShouldCompress = PyGenericIO.setDefaultShouldCompress
setNaturalDefaultPartition = PyGenericIO.setNaturalDefaultPartition
setDefaultFileIOType = PyGenericIO.setDefaultFileIOType
if not _GENERICIO_NO_MPI:
    setCollectiveMPIIOThreshold = PyGenericIO.setCollectiveMPIIOThreshold

# sensible defaults?
if not _GENERICIO_NO_MPI:
    setNaturalDefaultPartition()
    setDefaultFileIOType(FileIO.FileIOMPICollective)
