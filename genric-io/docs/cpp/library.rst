GenericIO and CMake
===================


Adding GenericIO as a dependency to a CMake C/C++ project is straight forward.
As an example, check out the `monofonIC <https://bitbucket.org/ohahn/monofonic>`_
library which has GenericIO as an optional dependency.

CMake >=3.11, <3.14
-------------------

If you're using ``CMake >= 3.11``, you can use the ``FetchContent`` routine to
download GenericIO at compile-time. Alternatively, you can include GenericIO as
a ``git`` submodule or directly copy the source code into the repository (not
recommended).

Here is an example for a very basic ``CMakeFile.txt``:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.11)
    project(TestGenericIO CXX)

    # Load GenericIO
    include(FetchContent)
    FetchContent_Declare(
        genericio
        GIT_REPOSITORY https://git.cels.anl.gov/hacc/genericio.git
        GIT_TAG master
        GIT_SHALLOW YES
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD TRUE   # <---- this is needed only for Ninja
    )

    FetchContent_GetProperties(genericio)
    if(NOT genericio_POPULATED)
        set(FETCHCONTENT_QUIET OFF)
        FetchContent_Populate(genericio)
        add_subdirectory(${genericio_SOURCE_DIR} ${genericio_BINARY_DIR})
    endif()

    # Add an executable
    add_executable(TestGenericIO test_genericio.cpp)

    # Link to GenericIO
    target_link_libraries(TestGenericIO PRIVATE genericio::genericio_mpi)

The last line will add the GenericIO headers to the include directories and
automatically link the library during compile time. If you want to compile your
program without the MPI library, you can link to the non-MPI version of
GenericIO: ``genericio::genericio``. If MPI is not available on your system,
then only the non-MPI version will be available as an option.

CMake >= 3.14
-------------

With more recent CMake versions, the ``CMakeLists.txt`` file can be simplified

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.14)
    project(TestGenericIO CXX)

    # Load GenericIO
    include(FetchContent)
    FetchContent_Declare(
        genericio
        GIT_REPOSITORY https://git.cels.anl.gov/hacc/genericio.git
        GIT_TAG master
        GIT_SHALLOW YES
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD TRUE   # <---- this is needed only for Ninja
    )
    FetchContent_MakeAvailable(genericio)

    # Add an executable
    add_executable(TestGenericIO test_genericio.cpp)

    # Link to GenericIO
    target_link_libraries(TestGenericIO PRIVATE genericio::genericio_mpi)
