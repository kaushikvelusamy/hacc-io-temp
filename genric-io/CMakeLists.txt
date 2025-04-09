cmake_minimum_required(VERSION 3.10)
project(GenericIO LANGUAGES C CXX)

# Are we the main project or in a nested directory?
set(GENERICIO_MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(GENERICIO_MASTER_PROJECT ON)
endif()

# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# PIC for everything (can be made target specific)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# MPI (if found: MPI_FOUND)
find_package(MPI)

# OpenMP  (if found: OpenMP_FOUND)
find_package(OpenMP)

# Options
# executables default: on if master project
if(MPI_FOUND)
    option(GENERICIO_MPI_EXECUTABLES "build MPI executables?" ${GENERICIO_MASTER_PROJECT})
else()
    set(GENERICIO_MPI_EXECUTABLES OFF)
endif()
option(GENERICIO_FRONTEND_EXECUTABLES "build frontend executables?" ${GENERICIO_MASTER_PROJECT})
option(GENERICIO_LEGACY_PYTHON_LIBRARY "build legacy python library?" ${GENERICIO_MASTER_PROJECT})
# new python target: only if cmake at least version 3.11
if(NOT (CMAKE_VERSION VERSION_LESS 3.11.0))
    option(GENERICIO_PYTHON_LIBRARY "build the python library with pybind11?" OFF)
else()
    message(WANRING " cmake version < 3.11.0, cannot build the python library")
    set(GENERICIO_PYTHON_LIBRARY OFF)
endif()

###############################################################################
# BLOSC
add_subdirectory(thirdparty)

###############################################################################
# GenericIO sources, libraries, and executables
set(GenericIO_Sources
    GenericIO.h GenericIO.cxx
    )

# Libraries: MPI and no MPI
add_library(genericio ${GenericIO_Sources})
add_library(genericio::genericio ALIAS genericio)
# for consumers: make GenericIO.h available (in current source dir)
target_include_directories(genericio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(genericio PUBLIC GENERICIO_NO_MPI)
target_link_libraries(genericio PRIVATE blosc)
if(OpenMP_FOUND)
    target_link_libraries(genericio PRIVATE OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
endif()
# change output to lib directory
set_target_properties(genericio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

if(MPI_FOUND)
    add_library(genericio_mpi ${GenericIO_Sources})
    add_library(genericio::genericio_mpi ALIAS genericio_mpi)
    # for consumers: make GenericIO.h available (in current source dir)
    target_include_directories(genericio_mpi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(genericio_mpi PUBLIC MPI::MPI_CXX)
    target_link_libraries(genericio_mpi PRIVATE blosc)
    # change output to lib directory
    set_target_properties(genericio_mpi PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    if(OpenMP_FOUND)
        target_link_libraries(genericio_mpi PRIVATE OpenMP::OpenMP_C OpenMP::OpenMP_CXX)
    endif()
endif()

# MPI Executables
if(GENERICIO_MPI_EXECUTABLES)
    set(MPI_Executables
        GenericIOPrint
        GenericIOVerify
        GenericIOBenchmarkRead
        GenericIOBenchmarkWrite
        GenericIORewrite
    )
    foreach(executable ${MPI_Executables})
        add_executable("${executable}_MPI" "${executable}.cxx")
        target_link_libraries("${executable}_MPI" PRIVATE genericio_mpi)
        set_target_properties("${executable}_MPI" PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/mpi"
            OUTPUT_NAME ${executable})
    endforeach()
endif()

# Frontend Executables
if(GENERICIO_FRONTEND_EXECUTABLES)
    set(Frontend_Executables
        GenericIOPrint
        GenericIOVerify
        )
    foreach(executable ${Frontend_Executables})
        add_executable(${executable} "${executable}.cxx")
        target_link_libraries(${executable} PRIVATE genericio)
        set_target_properties(${executable} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/frontend")
    endforeach()
endif()

# Legacy python library
if(GENERICIO_LEGACY_PYTHON_LIBRARY)
    add_library(pygio_legacy SHARED legacy_python/lib/gio.cxx legacy_python/lib/gio.h)
    target_link_libraries(pygio_legacy PRIVATE genericio)
    # GNUmakefile compatibility: also move to frontend (could be improved)
    set_target_properties(pygio_legacy PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/frontend")
    set_target_properties(pygio_legacy PROPERTIES OUTPUT_NAME pygio)
    # GNUmakefile compatibility: copy python files to build directory so that relative paths are correct
    file(COPY legacy_python/genericio.py legacy_python/example.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/legacy_python)
endif()

# Python library
if(GENERICIO_PYTHON_LIBRARY)
    add_subdirectory(python)
endif()