cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.11.1
)

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()

# the mpi version
if(MPI_FOUND)
pybind11_add_module(pygio_impl genericio.cpp)
target_link_libraries(pygio_impl PRIVATE genericio_mpi)
endif()

# The no-mpi version
pybind11_add_module(pygio_nompi_impl genericio.cpp)
target_link_libraries(pygio_nompi_impl PRIVATE genericio)