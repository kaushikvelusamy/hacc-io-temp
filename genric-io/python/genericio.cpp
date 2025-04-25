#include "GenericIO.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <optional>
#include <stdexcept>

#ifndef GENERICIO_NO_MPI
#include <mpi.h>
#endif

namespace py = pybind11;

class PyGenericIO : public gio::GenericIO {
public:
  PyGenericIO(
      const std::string& filename,
      gio::GenericIO::FileIO method=gio::GenericIO::FileIOPOSIX,
      gio::GenericIO::MismatchBehavior redistribute=gio::GenericIO::MismatchRedistribute,
      int eff_rank = -1)
#ifdef GENERICIO_NO_MPI
      : gio::GenericIO(filename, method), num_ranks(0) {
#else
      : gio::GenericIO(MPI_COMM_WORLD, filename, method), num_ranks(0) {
#endif
    // open headers and rank info
    openAndReadHeader(redistribute, eff_rank);
    num_ranks = readNRanks();
    // read variable info
    getVariableInfo(variables);
  }

  void inspect() {
    int rank;
  #ifdef GENERICIO_NO_MPI
    rank = 0;
  #else
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  #endif
    if(rank == 0) {
      std::stringstream s;
      s << "Number of Elements: " << readNumElems() << "\n";
      s << "Total number of Elements: " << readTotalNumElems() << "\n";
      s << "[data type] Variable name\n";
      s << "---------------------------------------------\n";
      for (int i = 0; i < variables.size(); ++i) {
        gio::GenericIO::VariableInfo vinfo = variables[i];
        if (vinfo.IsFloat)
          s << "[f";
        else
          s << "[i";
        int NumElements = vinfo.Size / vinfo.ElementSize;
        s << " " << vinfo.ElementSize * 8;
        if (NumElements > 1)
          s << "x" << NumElements;
        s << "] ";
        s << vinfo.Name << "\n";
      }
      s << "\n(i=integer,f=floating point, number bits size)\n";
      py::print(s.str());
    }
  }

  std::vector<std::string> read_variable_names() {
    std::vector<std::string> variable_names;
    for(const auto& v: variables) {
        variable_names.push_back(v.Name);
      }
    return variable_names;
  }

  std::map<std::string, py::dtype> read_variable_dtypes() {
    std::map<std::string, py::dtype> variable_dtypes;

    for(const auto& var: variables) {
      auto var_name = var.Name;
      if(var.IsFloat && var.ElementSize == 4)
        variable_dtypes[var_name] = py::dtype("f4");
      else if(var.IsFloat && var.ElementSize == 8)
        variable_dtypes[var_name] = py::dtype("f8");
      else if(!var.IsFloat && var.ElementSize == 4)
        variable_dtypes[var_name] = py::dtype("i4");
      else if(!var.IsFloat && var.ElementSize == 8)
        variable_dtypes[var_name] = py::dtype("i8");
      else if(!var.IsFloat && var.ElementSize == 2)
        variable_dtypes[var_name] = py::dtype("u2");
      else
        throw std::runtime_error(std::string("Unknown data type in GenericIO for variable ") + var_name);
    }
    return variable_dtypes;
  }

  std::map<std::string, py::array> read(
      std::optional<std::vector<std::string>> var_names,
      bool print_stats=true,
      bool collective_stats=true,
      int eff_rank=-1
    ) {
    int rank;
  #ifdef GENERICIO_NO_MPI
    rank = 0;
  #else
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  #endif

    // read number of elements
    int64_t num_elem = readNumElems(eff_rank);

    // if no argument, read all
    if(!var_names.has_value()) {
      var_names.emplace(read_variable_names());
    }

    clearVariables();
    std::map<std::string, py::array> result;

    for(const std::string& var_name: *var_names) {
      // check if it's not already defined (caused some segfaults at some point)
      if(result.count(var_name)) {
        throw std::invalid_argument(std::string("variable name was passed multiple times: ") + var_name);
      }
      auto varp = std::find_if(
        variables.begin(),
        variables.end(),
        [&var_name](const auto& v){ return v.Name == var_name; }
        );
      if (varp != variables.end()) {
        // extra space
        py::ssize_t readsize = num_elem + requestedExtraSpace()/(*varp).ElementSize;
        // py::array_t constructor: (shape, stride) -> some compilers or numpy
        // versions require stride to be explicitly specified
        if((*varp).IsFloat && (*varp).ElementSize == 4) {
          result[var_name] = py::array_t<float>({readsize}, {4});
          addVariable(*varp, result[var_name].mutable_data(), gio::GenericIO::VarHasExtraSpace);
        } else if((*varp).IsFloat && (*varp).ElementSize == 8) {
          result[var_name] = py::array_t<double>({readsize}, {8});
          addVariable(*varp, result[var_name].mutable_data(), gio::GenericIO::VarHasExtraSpace);
        } else if(!(*varp).IsFloat && (*varp).ElementSize == 4) {
          result[var_name] = py::array_t<int32_t>({readsize}, {4});
          addVariable(*varp, result[var_name].mutable_data(), gio::GenericIO::VarHasExtraSpace);
        } else if(!(*varp).IsFloat && (*varp).ElementSize == 8) {
          result[var_name] = py::array_t<int64_t>({readsize}, {8});
          addVariable(*varp, result[var_name].mutable_data(), gio::GenericIO::VarHasExtraSpace);
        } else if(!(*varp).IsFloat && (*varp).ElementSize == 2) {
          result[var_name] = py::array_t<uint16_t>({readsize}, {2});
          addVariable(*varp, result[var_name].mutable_data(), gio::GenericIO::VarHasExtraSpace);
        } else {
          throw std::runtime_error(std::string("unknown data type in GenericIO for variable: ") + var_name);
        }
      } else { // variable not found
        throw std::invalid_argument(std::string("requested variable is not defined in GenericIO file: ") + var_name);
      }
    }

    readData(eff_rank, print_stats, collective_stats);
    clearVariables();

  #ifndef GENERICIO_NO_MPI
    MPI_Barrier(MPI_COMM_WORLD);
  #endif

    // get rid of extraspace
    std::for_each(result.begin(), result.end(), [&](auto& item){ item.second.resize({num_elem}); });

    return result;
  }

  const std::vector<VariableInfo> &get_variables() {
    return variables;
  }

  std::array<double, 3> read_phys_origin() {
    std::array<double, 3> origin;
    readPhysOrigin(origin.data());
    return origin;
  }

  std::array<double, 3> read_phys_scale() {
    std::array<double, 3> scale;
    readPhysScale(scale.data());
    return scale;
  }

  std::vector<int> get_source_ranks() {
    std::vector<int> sr;
    getSourceRanks(sr);
    return sr;
  }

  std::array<int, 3> read_dims() {
    std::array<int, 3> sd;
    readDims(sd.data());
    return sd;
  }

  std::array<int, 3> read_coords(int eff_rank=-1) {
    std::array<int, 3> sc;
    readCoords(sc.data(), eff_rank);
    return sc;
  }

private:
  int num_ranks;
  std::vector<VariableInfo> variables;
};

std::map<std::string, py::array> read_genericio(
    std::string filename,
    std::optional<std::vector<std::string>> var_names,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute,
    bool print_stats=true,
    bool collective_stats=true,
    int eff_rank=-1
  ) {
  PyGenericIO reader(filename, method, redistribute, eff_rank);
  return reader.read(var_names, print_stats, collective_stats, eff_rank);
}

void inspect_genericio(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
  ) {
  PyGenericIO reader(filename, method, redistribute);
  reader.inspect();
}

std::array<double, 3> read_phys_scale(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.read_phys_scale();
}

std::array<double, 3> read_phys_origin(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.read_phys_origin();
}

std::array<int, 3> read_dims(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.read_dims();
}

std::array<int, 3> read_coords(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute,
    int eff_rank=-1
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.read_coords(eff_rank);
}

std::vector<std::string> read_variable_names(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.read_variable_names();
}

std::map<std::string, py::dtype> read_variable_dtypes(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.read_variable_dtypes();
}

int64_t read_num_elems(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute,
    int eff_rank=-1
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.readNumElems(eff_rank);
}

int64_t read_total_num_elems(
    std::string filename,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX,
    PyGenericIO::MismatchBehavior redistribute=PyGenericIO::MismatchBehavior::MismatchRedistribute
) {
  PyGenericIO reader(filename, method, redistribute);
  return reader.readTotalNumElems();
}

#ifndef GENERICIO_NO_MPI
void write_genericio(
    std::string filename,
    std::map<std::string, py::array> variables,
    std::array<double, 3> phys_scale, std::array<double, 3> phys_origin,
    PyGenericIO::FileIO method=PyGenericIO::FileIO::FileIOPOSIX
  ) {
  // check data integrity, find particle count
  int64_t particle_count = -1;
  for(auto const& [name, data]: variables) {
    if(data.ndim() != 1) {
      throw std::runtime_error("dimension of array must be 1 (" + name + ")");
    }
    if(particle_count == -1) {
      particle_count = data.size();
    } else if(particle_count != data.size()) {
      throw std::runtime_error("arrays do not have same length (" + name + ")");
    }
  }

  gio::GenericIO writer(MPI_COMM_WORLD, filename, method);

  writer.setNumElems(particle_count);

  // set size
  for (int d = 0; d < 3; ++d) {
    writer.setPhysOrigin(phys_origin[d], d);
    writer.setPhysScale(phys_scale[d], d);
  }

  for(auto& [name, data]: variables) {
    if(py::isinstance<py::array_t<float>>(data))
      writer.addVariable(name.c_str(), reinterpret_cast<float*>(data.mutable_data()));
    else if(py::isinstance<py::array_t<double>>(data))
      writer.addVariable(name.c_str(), reinterpret_cast<double*>(data.mutable_data()));
    else if(py::isinstance<py::array_t<int32_t>>(data))
      writer.addVariable(name.c_str(), reinterpret_cast<int32_t*>(data.mutable_data()));
    else if(py::isinstance<py::array_t<int64_t>>(data))
      writer.addVariable(name.c_str(), reinterpret_cast<int64_t*>(data.mutable_data()));
    else if(py::isinstance<py::array_t<uint16_t>>(data))
      writer.addVariable(name.c_str(), reinterpret_cast<uint16_t*>(data.mutable_data()));
    else
      throw std::runtime_error("array dtype not supported for " + name);
  }
  writer.write();
  MPI_Barrier(MPI_COMM_WORLD);

}
#endif


#ifdef GENERICIO_NO_MPI
PYBIND11_MODULE(pygio_nompi_impl, m) {
  m.doc() = "genericio python module (no MPI support)";
#else  // GENERICIO_NO_MPI
PYBIND11_MODULE(pygio_impl, m) {
  m.doc() = "genericio python module (with MPI support)";
  m.def("_init_mpi", [](){
    int initialized;
    MPI_Initialized(&initialized);
    if(!initialized) {
      int level_provided;
      MPI_Init_thread(nullptr, nullptr, MPI_THREAD_SINGLE, &level_provided);
    }
  });
#endif

  py::class_<PyGenericIO> pyGenericIO(m, "PyGenericIO");

  // ENUMS
  py::enum_<PyGenericIO::FileIO>(pyGenericIO, "FileIO")
    .value("FileIOMPI", PyGenericIO::FileIO::FileIOMPI)
    .value("FileIOPOSIX", PyGenericIO::FileIO::FileIOPOSIX)
    .value("FileIOMPICollective", PyGenericIO::FileIO::FileIOMPICollective);

  py::enum_<PyGenericIO::MismatchBehavior>(pyGenericIO, "MismatchBehavior")
    .value("MismatchAllowed", PyGenericIO::MismatchBehavior::MismatchAllowed)
    .value("MismatchDisallowed", PyGenericIO::MismatchBehavior::MismatchDisallowed)
    .value("MismatchRedistribute", PyGenericIO::MismatchBehavior::MismatchRedistribute);

  pyGenericIO.def(
      py::init<std::string, PyGenericIO::FileIO, PyGenericIO::MismatchBehavior>(),
        py::arg("filename"),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute)
      .def("inspect", &PyGenericIO::inspect, "Print variable infos and size of GenericIO file")
      .def("get_variables", &PyGenericIO::get_variables, "Get a list of VariableInformations defined in the GenericIO file")
      .def("read_num_elems", (size_t (PyGenericIO::*)(int))(&PyGenericIO::readNumElems), py::arg("eff_rank")=-1)
      .def("read_total_num_elems", (uint64_t (PyGenericIO::*)(void))(&PyGenericIO::readTotalNumElems))
      .def("read_phys_origin", &PyGenericIO::read_phys_origin)
      .def("read_phys_scale", &PyGenericIO::read_phys_scale)
      .def("read_dims", &PyGenericIO::read_dims)
      .def("read_coords", &PyGenericIO::read_coords)
      .def("read", &PyGenericIO::read,
        py::arg("variables")=nullptr,
        py::kw_only(),
        py::arg("print_stats")=true,
        py::arg("collective_stats")=true,
        py::arg("eff_rank")=-1)
      .def("read_nranks", (int (PyGenericIO::*)(void))(&PyGenericIO::readNRanks))
      .def("read_variable_names", &PyGenericIO::read_variable_names)
      .def("has_variable", &PyGenericIO::hasVariable)
      .def("read_variable_dtypes", &PyGenericIO::read_variable_dtypes)
      .def("get_source_ranks", &PyGenericIO::getSourceRanks)
      .def_static("setDefaultShouldCompress", &PyGenericIO::setDefaultShouldCompress)
      .def_static("setNaturalDefaultPartition", &PyGenericIO::setNaturalDefaultPartition)
      .def_static("setDefaultFileIOType", &PyGenericIO::setDefaultFileIOType)
#ifndef GENERICIO_NO_MPI
      .def_static("setCollectiveMPIIOThreshold", &PyGenericIO::setCollectiveMPIIOThreshold)
#endif
      ;

  py::class_<PyGenericIO::VariableInfo>(pyGenericIO, "VariableInfo")
      .def_readonly("name", &gio::GenericIO::VariableInfo::Name)
      .def_readonly("size", &gio::GenericIO::VariableInfo::Size)
      .def_readonly("element_size", &gio::GenericIO::VariableInfo::ElementSize)
      .def_readonly("is_float", &gio::GenericIO::VariableInfo::IsFloat)
      .def("__repr__", [](const gio::GenericIO::VariableInfo &vi) {
        return std::string("<PyGenericIO.VariableInfo type=") +
               (vi.IsFloat ? "float" : "int") + " name='" + vi.Name + "'>";
      });


  m.def("read_genericio", &read_genericio, R"Delim(
        Read data from a GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        variables: List[str]
            A list of variable names that should be read. If ``None``, all
            variables contained in the file will be read

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        print_stats: bool
            if ``True``, print throughput statistics after reading

        collective_stats: bool
            if ``True``, aggregate statistics among reading ranks (if using MPI)

        eff_rank: int
            if -1, use the MPI rank of the calling process (or redistribute if
            redistribute is set to MismatchRedistribute). Otherwise, use the
            specified rank (if using MPI).

        Returns
        -------
        data: Mapping[str, np.ndarray]

  )Delim",
        py::arg("filename"),
        py::arg("variables")=nullptr,
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute,
        py::arg("print_stats")=true,
        py::arg("collective_stats")=true,
        py::arg("eff_rank")=-1,
        py::return_value_policy::move);


  m.def("inspect_genericio", &inspect_genericio, R"Delim(
        Print a summary of variables and types defined in the GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

  m.def("read_num_elems", &read_num_elems, R"Delim(
        Read the (local) number of objects (the number of objects that would be
        read by this rank when calling :func:`read_genericio`)

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        eff_rank: int
            if -1, use the MPI rank of the calling process (or redistribute if
            redistribute is set to MismatchRedistribute). Otherwise, use the
            specified rank (if using MPI).

        Returns
        -------
        nlocal: int
            the number of objects assigned to this rank
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute,
        py::arg("eff_rank")=-1);

  m.def("read_total_num_elems", &read_total_num_elems, R"Delim(
        Read the total number of objects (the number of objects that would be
        read by all ranks combined when calling :func:`read_genericio`)

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        Returns
        -------
        ntotal: int
            the total number of objects stored in the GenericIO file
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

  m.def("read_variable_names", &read_variable_names, R"Delim(
        Get a list of variable names stored in the GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        Returns
        -------
        variable_names: List[str]
            the list of variable names defined in the GenericIO file
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

  m.def("read_variable_dtypes", &read_variable_dtypes, R"Delim(
        Get a dictionary of dtypes mapped to the variable names

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        Returns
        -------
        variable_dtypes: Mapping[str, np.dtype]
            a map ``variable_name -> dtype`` for each variable in the GenericIO file
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

  m.def("read_phys_scale", &read_phys_scale, R"Delim(
        Read the box size that is stored in the GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        Returns
        -------
        phys_scale: List[float]
            the box length for each dimension (3 elements long)
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

  m.def("read_phys_origin", &read_phys_scale, R"Delim(
        Read the origin / reference point of the box that is stored in the GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        Returns
        -------
        phys_origin: List[float]
            the box origin coordinates (3 elements long)
  )Delim",
        py::arg("filename"),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
        py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

m.def("read_dims", &read_dims, R"Delim(
        Read the topological decomposition of the box that is stored in the GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        Returns
        -------
        dims: List[int]
            the box decomposition (3 elements long)
  )Delim",
  py::arg("filename"),
  py::kw_only(),
  py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
  py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute);

m.def("read_coords", &read_coords, R"Delim(
        Read the rank coordinates of the source_rank this rank is reading

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        method: PyGenericIO.FileIO
            file handling method (POSIX)

        redistribute: PyGenericIO.MismatchBehavior
            whether to allow mismatching ranks

        eff_rank: int
            if -1, use the MPI rank of the calling process (or redistribute if
            redistribute is set to MismatchRedistribute). Otherwise, use the
            specified rank (if using MPI).

        Returns
        -------
        coords: List[int]
            the rank coordinates (3 elements long)

        Note
        ----
        If eff_rank == -1 and redistribute is enabled, the rank coordinates
        will be [0, 0, 0]
  )Delim",
  py::arg("filename"),
  py::kw_only(),
  py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX,
  py::arg("redistribute")=PyGenericIO::MismatchBehavior::MismatchRedistribute,
  py::arg("eff_rank")=-1);


#ifndef GENERICIO_NO_MPI
  m.def("write_genericio", &write_genericio, R"Delim(
        Write data as a GenericIO file

        Parameters
        ----------
        filename: str
            path to the GenericIO file

        data: Mapping[str, np.ndarray]
            a dictionary, with all items being 1-dimensional numpy arrays of
            the same length. Currently, only float32, float64, int32, int64 and
            uint16 data types are supported

        phys_scale: List[float]
            the physical size of the box that the data belongs to (3 elements)

        phys_origin: List[float]
            the origin coordinates of the box that the data belongs to (3 elements)

        method: PyGenericIO.FileIO
            file handling method (POSIX/MPI)
  )Delim",
        py::arg("filename"),
        py::arg("data"),
        py::arg("phys_scale"),
        py::arg("phys_origin") = std::array<double, 3>({0., 0., 0.}),
        py::kw_only(),
        py::arg("method")=PyGenericIO::FileIO::FileIOPOSIX);
#endif
}
